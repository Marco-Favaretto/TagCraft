#include "controller/appcontroller.h"

#include "db/databasemanager.h"
#include "db/entitymapper.h"
#include "storage/storagemanager.h"
#include "dao/trackdao.h"
#include "utils/imageutils.h"
#include "dto/constants.h"

#include <QImage>
#include <QMutex>
#include <QtConcurrent>
#include <QDesktopServices>
#include <QFileInfo>
#include <QUrl>
#include <QDir>
#include <QFile>

AppController::AppController(QObject* parent) : QObject(parent) {}

bool AppController::initialize() {
    if (!StorageManager::instance().scanForStorage()) {
        emit errorOccurred("Storage esterno non trovato");
    }

    QString dbPath = StorageManager::instance().musicAppPoint() + "/" + Constants::Paths::DatabaseFileName;
    if (!DatabaseManager::instance().openDatabase(dbPath)) {
        emit errorOccurred("Impossibile aprire il database");
        return false;
    }
    if(!DatabaseManager::instance().initSchema()) {
        emit errorOccurred("Impossibile inizializzare il database, errore nella creazione dello schema o dei valori di default.");
        return false;
    }

    m_storageController = new StorageController(this);
    m_metadataController = new MetadataController(this);
    m_databaseController = new DatabaseController(this);

    setupConnections();

    emit appReady();
    return true;
}

void AppController::setupConnections() {
    connect(m_storageController, &StorageController::scanFinished,
            this, &AppController::onScanFinished);
    connect(m_storageController, &StorageController::fullScanFinished,
            this, &AppController::onFullScanFinished);
    connect(m_storageController, &StorageController::scanProgress,
            this, &AppController::scanProgress);
    connect(m_storageController, &StorageController::errorOccurred,
            this, &AppController::errorOccurred);
    connect(m_storageController, &StorageController::storageMounted,
            this, &AppController::onStorageMounted);
    connect(m_storageController, &StorageController::storageUnmounted,
            this, &AppController::onStorageUnmounted);
    connect(m_metadataController, &MetadataController::metadataSaved,
            this, &AppController::metadataSaved);
    connect(m_metadataController, &MetadataController::metadataSaveFailed,
            this, &AppController::metadataSaveFailed);
    connect(m_metadataController, &MetadataController::errorOccurred,
            this, &AppController::errorOccurred);
    connect(m_databaseController, &DatabaseController::persistProgress,
        this, [this](int current, int total) {
            int percentage = (total > 0) ? static_cast<int>((current * 100.0) / total) : 0;
            emit scanProgress(percentage);
        });
}

LibraryController* AppController::library() const { return m_libraryController; }

MetadataController* AppController::metadata() const { return m_metadataController; }

void AppController::requestScan(const QString& path) {
    m_storageController->runScan(path);
}

void AppController::requestSaveMetadata(const QString& relativePath, const TrackDto& newValues) {
    if (m_metadataController->saveMetadata(relativePath, newValues)) {
        QFileInfo info(StorageManager::instance().toAbsolutePath(relativePath));
        QList<TrackFileSystemDto> list;
        list.append({relativePath, info.size(), info.lastModified().toSecsSinceEpoch()});
        if(m_databaseController->updateNewTracks(list)) emit libraryUpdated();
        else emit errorOccurred("errore nell'aggiornamento della traccia");
    }
}

void AppController::requestSaveMetadataBatch(const QList<QString>& relativePaths, const TrackDto& newValues) {
    for(const QString& s : relativePaths) this->requestSaveMetadata(s, newValues);
}

void AppController::requestSetCover(const QString& relativePath, const QString& imagePath) {
    QImage image(imagePath);
    if (image.isNull()) {
        emit errorOccurred("L'immagine è null");
        return;
    }

    const QString artworkHash = ImageUtils::contentHash(image);
    if (artworkHash.isEmpty()) {
        emit errorOccurred("Hash è empty");
        return;
    }

    const QString cachedPath = ImageUtils::cacheArtwork(image, StorageManager::instance().artworkCacheDirectory());
    if (cachedPath.isEmpty()) {
        emit errorOccurred("cachedPath è empty");
        return;
    }

    if(!m_metadataController->setCover(relativePath, imagePath)) {
        emit errorOccurred("errore nel set della cover nei metadata");
        return;
    }

    std::optional<Track> t = TrackDao::findByRelativePath(relativePath);
    if(!t) {
        emit errorOccurred("Errore nel recupero della traccia " + relativePath);
        return;
    }

    if(!TrackDao::updateCover(t->id(), cachedPath)) {
        emit errorOccurred("errore nell'update della cover a db");
        return;
    }
}

void AppController::requestRemoveCover(const QString& relativePath) {
    std::optional<Track> t = TrackDao::findByRelativePath(relativePath);
    if(!t) {
        emit errorOccurred("Errore nel recupero della traccia " + relativePath);
        return;
    }
    if(m_metadataController->removeCover(relativePath)) {
        if(!TrackDao::updateCover(t->id(), "NULL")) {
            emit errorOccurred("errore nell'update della cover a db");
            return;
        }
    } else emit errorOccurred("Errore nell'eliminazione della traccia");

    emit libraryUpdated();
}

void AppController::requestCleanTags(const QString& relativePath) {
    std::optional<Track> t = TrackDao::findByRelativePath(relativePath);
    if(!t) {
        emit errorOccurred("Errore nel recupero della traccia " + relativePath);
        return;
    }
    if(m_metadataController->cleanTags(relativePath)) {
        if(!TrackDao::cleanTags(t->id())) {
            emit errorOccurred("errore nell'update della cover a db");
            return;
        }
    } else emit errorOccurred("Errore nell'eliminazione della traccia");

    emit libraryUpdated();
}

void AppController::requestSetCoverBatch(const QList<QString>& relativePaths, const QString& imagePath) {
    for(auto path : relativePaths) this->requestSetCover(path, imagePath);
}

void AppController::onScanFinished(const ScanResultDto& result) {
    qDebug() << "SmartScan Terminata, sincronizzazione DB";

    if (!result.newTracks.isEmpty()) {
        if (!m_databaseController->insertNewTracks(result.newTracks))
            emit errorOccurred("Inserimento nuove tracce fallito");
        else{
            qDebug() << "Risoluzione copertine tracce nuove";
            resolveArtworkFor(result.newTracks);
        }
    }

    if (!result.modifiedTracks.isEmpty()) {
        if (!m_databaseController->updateNewTracks(result.modifiedTracks))
            emit errorOccurred("Aggiornamento tracce fallito");
        else
            resolveArtworkFor(result.modifiedTracks);
    }

    if (!result.deletedTracks.isEmpty() && !m_databaseController->deleteNewTracks(result.deletedTracks))
        emit errorOccurred("Eliminazione tracce fallito");

    qDebug() << "Associazione copertina prima traccia con copertina album";
    if(!m_databaseController->syncAlbumCovers())
        emit errorOccurred("Sincronizzazione cover album fallita");

    emit libraryUpdated();
}

void AppController::onStorageMounted(const QString& mountPoint) {
    emit storageMounted(mountPoint);
}
void AppController::onStorageUnmounted() {
    emit storageUnmounted();
}

void AppController::requestResetDb() {
    if(m_databaseController->resetDb()) emit libraryUpdated();
    else emit errorOccurred("errore durante il reset del db");
}

void AppController::requestResetAndRebuildDb() {
    if (!m_databaseController->resetDb()) {
        emit errorOccurred("Reset del database fallito");
        return;
    }
    m_storageController->runFullScan(StorageManager::instance().musicPoint()); 
}

void AppController::onFullScanFinished(const QList<TrackFileSystemDto>& list) {
    if (!m_databaseController->insertNewTracks(list)) {
        emit errorOccurred("Ricostruzione database fallita");
    } else {
        qDebug() << "Risoluzione copertine tracce nuove";
        resolveArtworkFor(list);
        qDebug() << "Associazione copertina prima traccia con copertina album";
        if (!m_databaseController->syncAlbumCovers())
            emit errorOccurred("Sincronizzazione cover album fallita");
    }
    
    emit libraryUpdated();
}

// QtConcurrent -> passa da 15m34 per 4k tracce a 4m
void AppController::resolveArtworkFor(const QList<TrackFileSystemDto>& tracks) {
    struct Result {
        QString path;
        QString hash;
    };

    // Mutex: un "lucchetto" che garantisce che solo UN thread alla volta
    // possa eseguire il blocco di codice protetto da esso. Senza, se due
    // thread scrivessero contemporaneamente su `results` (una QList, che
    // NON è thread-safe), rischieresti corruzione di memoria o crash.
    QMutex mutex;
    QList<Result> results;
    QThreadPool pool;
    pool.setMaxThreadCount(qMax(1, QThread::idealThreadCount() - 1));

    // blockingMap: prende la lista `tracks` e applica la lambda a ogni
    // elemento, distribuendo il lavoro su un pool di thread gestito
    // automaticamente da Qt (di solito quanti sono i core della CPU).
    // "blocking" significa che questa chiamata NON ritorna finché TUTTI
    // gli elementi non sono stati processati — il thread principale (UI)
    // resta fermo qui ad aspettare, esattamente come una chiamata sincrona
    // normale, solo che il lavoro viene svolto in parallelo internamente.
    QtConcurrent::blockingMap(
        &pool,
        tracks,
        // Questa lambda viene eseguita N volte in parallelo (una per
        // traccia), CIASCUNA su un thread diverso del pool. Non hai
        // controllo su quale traccia va su quale thread, né sull'ordine.
        [this, &mutex, &results](const TrackFileSystemDto& track) {
            // Questa parte è quella "costosa" (I/O su disco, TagLib,
            // calcolo hash, scrittura file thumbnail) — è il motivo per
            // cui vale la pena parallelizzarla: ogni traccia è indipendente
            // dalle altre, nessuna ha bisogno del risultato di un'altra.
            const QString hash = m_metadataController->resolveAndCacheArtwork(track.relativePath);

            // QMutexLocker: acquisisce il lucchetto (mutex) all'inizio
            // dello scope e lo rilascia automaticamente alla fine dello
            // scope (fine del blocco `if`) — stesso principio RAII di
            // TransactionManager che avevamo usato per le transazioni DB.
            // Qui è indispensabile perché stiamo per modificare
            // `results`, che è condivisa da tutti i thread.
            if (!hash.isEmpty()) {
                QMutexLocker locker(&mutex);
                results.append({track.relativePath, hash});
            }
        }
    );

    for (const auto& result : results) {
        m_databaseController->updateTrackCoverHash(
            result.path,
            result.hash);
    }
}

void AppController::openFS(const QString& relativePath, bool isAlbum) {
    const QString absolutePath = m_storageController->resolveToAbsolutePath(relativePath);
    QFileInfo fileInfo(absolutePath);
    if (!fileInfo.exists()) {
        qWarning() << "Path non esiste:" << absolutePath;
        return;
    }

    QString directoryPath;
    if (isAlbum) {
        if (!fileInfo.isDir()) {
            qWarning() << "errore nel path:" << absolutePath;
            return;
        }
        directoryPath = fileInfo.absoluteFilePath();
    } else {
        if (!fileInfo.isFile()) {
            qWarning() << "Expected file, got:" << absolutePath;
            return;
        }
        directoryPath = fileInfo.absolutePath();
    }

    if (!QDesktopServices::openUrl(QUrl::fromLocalFile(directoryPath))) {
        qWarning() << "Impossibile aprire file manager:" << directoryPath;
    }
}

void AppController::deleteFromFS(const QString& relativePath, bool isAlbum) {
    const QString absolutePath = m_storageController->resolveToAbsolutePath(relativePath);
    QList<QString> list;
    bool success = false;
    if (isAlbum) {
        QDir dir(absolutePath);
        success = dir.removeRecursively();
        list = m_libraryController->getRPathTracksFromAlbumRPath(relativePath);
    } else {
        success = QFile::remove(absolutePath);
        list.append(relativePath);
    }

    qDebug() << "Allineamento db";
    if(!m_databaseController->deleteNewTracks(list)) emit errorOccurred("Sincronizzazione db fallita");
    else emit libraryUpdated();

    if (!success) {
        qWarning() << "Impossibile eliminare:" << absolutePath;
    }
}
