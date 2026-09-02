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
        m_databaseController->updateNewTracks(list);
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

// void AppController::resolveArtworkFor(const QList<TrackFileSystemDto>& tracks) {
    //     for (const auto& t : tracks) {
        //         QString hash = m_metadataController->resolveAndCacheArtwork(t.relativePath);
        //         qDebug() << "path: " << t.relativePath << ", hash: " << hash;
        //         if (!hash.isEmpty())
        //             m_databaseController->updateTrackCoverHash(t.relativePath, hash);
        //     }
        // }
        
void AppController::resolveArtworkFor(const QList<TrackFileSystemDto>& tracks) {
    struct Result {
        QString path;
        QString hash;
    };

    QMutex mutex;
    QList<Result> results;

    QtConcurrent::blockingMap(
        tracks,
        [this, &mutex, &results](const TrackFileSystemDto& track) {
            const QString hash =
                m_metadataController->resolveAndCacheArtwork(track.relativePath);

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
