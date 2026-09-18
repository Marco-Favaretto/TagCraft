#include "controller/appcontroller.h"

#include "db/databasemanager.h"
#include "db/entitymapper.h"
#include "storage/storagemanager.h"
#include "dao/trackdao.h"
#include "dao/albumdao.h"
#include "dao/artistdao.h"
#include "dao/genredao.h"
#include "utils/imageutils.h"
#include "dto/constants.h"
#include "ui/editmodels/trackeditmodel.h"
#include "ui/editmodels/albumeditmodel.h"

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
    m_storageController = new StorageController(this);
    m_metadataController = new MetadataController(this);
    m_databaseController = new DatabaseController(this);
    setupConnections();

    if (!tryMountAndOpenDatabase()) {
        emit errorOccurred("Storage esterno non trovato. Collega un dispositivo e usa 'Scan For Devices'.");
        return true;
    }

    emit appReady();
    return true;
}

bool AppController::tryMountAndOpenDatabase() {
    if (!m_storageController->isStorageMounted()) {
        if (!m_storageController->scanForStorage()) return false;
    }

    const QString dbPath = StorageManager::instance().musicAppPoint() + "/" + Constants::Paths::DatabaseFileName;
    if (!DatabaseManager::instance().openDatabase(dbPath)) {
        emit errorOccurred("Impossibile aprire il database");
        return false;
    }
    if (!DatabaseManager::instance().initSchema()) {
        emit errorOccurred("Impossibile inizializzare il database, errore nella creazione dello schema o dei valori di default.");
        return false;
    }
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
    // connect(m_storageController, &StorageController::storageMounted,
    //         this, &AppController::storageMounted);
    connect(m_storageController, &StorageController::storageUnmounted,
            this, &AppController::storageUnmounted);
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

void AppController::requestScanForDevices() {
    if (m_storageController->isStorageMounted()) {
        emit errorOccurred("Storage already mounted at " + m_storageController->currentMountPoint());
        return;
    }

    if (!tryMountAndOpenDatabase()) {
        emit errorOccurred("Errore nella scan degli storage");
        return;
    }

    emit storageMounted(StorageManager::instance().mountPoint());
}

void AppController::requestSaveMetadata(const QString& relativePath, const TrackDto& newValues) {
    auto trackOpt = TrackDao::findByRelativePath(relativePath);
    const QList<QPair<QString, TrackDto>> snapshot = trackOpt
        ? snapshotTracks({*trackOpt})
        : QList<QPair<QString, TrackDto>>{};

    const QString error = saveMetadataFileCore(relativePath, newValues);
    if (!error.isEmpty()) {
        emit batchOperationFinished("Modifica metadati", 0, 1);
        emit errorOccurred(error);
        return;
    }

    QFileInfo info(m_storageController->resolveToAbsolutePath(relativePath));
    QList<TrackFileSystemDto> list;
    list.append({relativePath, info.size(), info.lastModified().toSecsSinceEpoch()});

    if (m_databaseController->updateNewTracks(list)) {
        emit batchOperationFinished("Modifica metadati", 1, 0);
        setLastAction("Annulla: modifica metadati", {relativePath}, [this, snapshot]() {
            restoreTrackSnapshots(snapshot);
        });
        emit libraryUpdated();
    } else {
        emit batchOperationFinished("Modifica metadati", 0, 1);
        emit errorOccurred("errore nell'aggiornamento della traccia nel db");
    }
}

void AppController::requestSaveMetadataBatch(const QList<Track>& tracks, const QHash<QString, QVariant>& changedValues) {
    const QList<QPair<QString, TrackDto>> snapshot = snapshotTracks(tracks);

    QList<TrackFileSystemDto> updatedFiles;
    auto [succeeded, failed] = saveMetadataBatchCore(tracks, changedValues, updatedFiles);

    emit batchOperationFinished("Modifica metadati", succeeded, failed);

    if (updatedFiles.isEmpty()) return;

    if (m_databaseController->updateNewTracks(updatedFiles)) emit libraryUpdated();
    else emit errorOccurred("errore nell'aggiornamento delle tracce nel db");

    if (failed == 0 && !snapshot.isEmpty()) {
        QSet<QString> paths;
        for (const auto& s : snapshot) paths.insert(s.first);
        setLastAction("Annulla: modifica metadati batch", paths, [this, snapshot]() {
            restoreTrackSnapshots(snapshot);
        });
    }
}

void AppController::requestSetCover(const QString& relativePath, const QString& imagePath) {
    auto trackOpt = TrackDao::findByRelativePath(relativePath);
    const QList<QPair<QString, TrackDto>> snapshot = trackOpt
        ? snapshotTracks({*trackOpt})
        : QList<QPair<QString, TrackDto>>{};
    
    const QString error = setCoverCore(relativePath, imagePath);
    if (!error.isEmpty()) {
        emit batchOperationFinished("Imposta artwork", 0, 1);
        emit errorOccurred(error);
        return;
    }
    emit batchOperationFinished("Imposta artwork", 1, 0);
    setLastAction("Annulla: imposta artwork", {relativePath}, [this, snapshot]() {
        restoreTrackSnapshots(snapshot); // se non aveva cover -> removeCoverCore; se ne aveva una -> la re-imposta dalla cache
    });
    emit libraryUpdated();
}

void AppController::requestRemoveCover(const QString& relativePath) {
    auto trackOpt = TrackDao::findByRelativePath(relativePath);
    const QList<QPair<QString, TrackDto>> snapshot = trackOpt
        ? snapshotTracks({*trackOpt})
        : QList<QPair<QString, TrackDto>>{};

    const QString error = removeCoverCore(relativePath);
    if (!error.isEmpty()) {
        emit batchOperationFinished("Rimuovi artwork", 0, 1);
        emit errorOccurred(error);
        return;
    }
    emit batchOperationFinished("Rimuovi artwork", 1, 0);
    setLastAction("Annulla: imposta artwork", {relativePath}, [this, snapshot]() {
        restoreTrackSnapshots(snapshot);
    });
    emit libraryUpdated();
}

void AppController::requestCleanTags(const QString& relativePath) {
    auto trackOpt = TrackDao::findByRelativePath(relativePath);
    const QList<QPair<QString, TrackDto>> snapshot = trackOpt
        ? snapshotTracks({*trackOpt})
        : QList<QPair<QString, TrackDto>>{};

    const QString error = cleanTagsCore(relativePath);
    if (!error.isEmpty()) {
        emit batchOperationFinished("Clean Tags", 0, 1);
        emit errorOccurred(error);
        return;
    }
    emit batchOperationFinished("Clean Tags", 1, 0);
    setLastAction("Annulla: Clean Tags", {relativePath}, [this, snapshot]() {
        restoreTrackSnapshots(snapshot);
    });
    emit libraryUpdated();
}

void AppController::requestSetCoverBatch(const QList<QString>& relativePaths, const QString& imagePath) {
    const QList<QPair<QString, TrackDto>> snapshot = snapshotTracks(m_libraryController->getTracksFromRelativePaths(relativePaths));

    int succeeded = 0, failed = 0;
    for (const QString& path : relativePaths) {
        if (setCoverCore(path, imagePath).isEmpty()) ++succeeded; 
        else ++failed;
    }
    emit batchOperationFinished("Imposta artwork", succeeded, failed);
    if (succeeded > 0) emit libraryUpdated();

    if (failed == 0 && !snapshot.isEmpty()) {
        QSet<QString> paths;
        for (const auto& s : snapshot) paths.insert(s.first);
        setLastAction("Annulla: modifica metadati batch", paths, [this, snapshot]() {
            restoreTrackSnapshots(snapshot);
        });
    }
}

void AppController::requestSetAlbumCover(int id, const QString& imagePath) {
    QList<QString> trackPaths;
    for (auto t : m_libraryController->getTracksByAlbum(id)) trackPaths.append(t.relativePath());
    requestSetCoverBatch(trackPaths, imagePath);
}

void AppController::requestRemoveAlbumCover(int id) {
    QList<QString> trackPaths;
    for (auto t : m_libraryController->getTracksByAlbum(id)) trackPaths.append(t.relativePath());
    requestRemoveCoverBatch(trackPaths);
}

// result["id"], result["title"], result["artistName"], result["genreName"], result["trackNumbers"]
void AppController::requestSaveAlbumMetadata(const QHash<QString, QVariant>& albumChanges) {
    const int albumId = albumChanges.value("id").toInt();
    const QString newTitle = albumChanges.value(AlbumEditModel::KeyTitle).toString();
    const QString newArtistName = albumChanges.value(AlbumEditModel::KeyArtist).toString();
    const QString newGenreName = albumChanges.value(AlbumEditModel::KeyGenre).toString();

    auto albumOpt = m_libraryController->getAlbumById(albumId);
    const QString oldTitle = albumOpt ? albumOpt->title() : QString();
    const int oldArtistId = albumOpt ? albumOpt->artistId() : Constants::DefaultValues::ArtistId;
    const int oldGenreId = albumOpt ? albumOpt->genreId() : Constants::DefaultValues::GenreId;
    const QList<Track> tracks = m_libraryController->getTracksByAlbum(albumId);
    const QList<QPair<QString, TrackDto>> snapshot = snapshotTracks(tracks);

    QHash<QString, QVariant> changed;
    changed.insert(TrackEditModel::KeyAlbum, newTitle);
    changed.insert(TrackEditModel::KeyArtist, newArtistName);
    changed.insert(TrackEditModel::KeyGenre, newGenreName);

    QList<TrackFileSystemDto> updatedFiles;
    auto [succeeded, failed] = saveMetadataBatchCore(m_libraryController->getTracksByAlbum(albumId), changed, updatedFiles);

    const int artistId = m_databaseController->resolveArtistId(newArtistName);
    const int genreId = m_databaseController->resolveGenreId(newGenreName);

    if (!AlbumDao::updateMetadata(albumId, newTitle, artistId, genreId)) {
        emit batchOperationFinished("Modifica album", succeeded, failed + 1);
        emit errorOccurred("errore nell'aggiornamento dell'album nel db");
        return;
    }

    const QVariantMap newTrackNumbers = albumChanges.value(AlbumEditModel::KeyTrackNumbers).toMap();
    for (auto it = newTrackNumbers.constBegin(); it != newTrackNumbers.constEnd(); ++it) {
        const int trackId = it.key().toInt();
        const int newNumber = it.value().toInt();
        auto trackOpt = m_libraryController->getTrackById(trackId);
        if (!trackOpt) { ++failed; continue; }

        TrackEditModel model(*trackOpt, m_libraryController);
        QHash<QString, QVariant> numberChange;
        numberChange.insert(TrackEditModel::KeyTrackNumber, newNumber);
        const TrackDto dto = model.buildDto(numberChange);

        if (saveMetadataFileCore(trackOpt->relativePath(), dto).isEmpty()) {
            QFileInfo info(m_storageController->resolveToAbsolutePath(trackOpt->relativePath()));
            updatedFiles.append({trackOpt->relativePath(), info.size(), info.lastModified().toSecsSinceEpoch()});
            ++succeeded;
        } else ++failed;
    }

    emit batchOperationFinished("Modifica album", succeeded, failed);

    if (!updatedFiles.isEmpty() && !m_databaseController->updateNewTracks(updatedFiles)) emit errorOccurred("errore nell'aggiornamento delle tracce nel db");

    if (failed == 0) {
        QSet<QString> paths;
        for (const auto& s : snapshot) paths.insert(s.first);

        setLastAction(QString("Annulla: modifica album \"%1\"").arg(oldTitle), paths,
            [this, albumId, oldTitle, oldArtistId, oldGenreId, snapshot]() {
                AlbumDao::updateMetadata(albumId, oldTitle, oldArtistId, oldGenreId);
                restoreTrackSnapshots(snapshot); // ripristino anche dei nuovi track numbers
            });
    }

    emit libraryUpdated();
}

void AppController::requestRenameArtist(int id, const QString& newName) {
    auto artistOpt = m_libraryController->getArtistById(id);
    const QString oldName = artistOpt ? artistOpt->name() : QString();
    const QList<Track> tracks = m_libraryController->getTracksByArtist(id);
    const QList<QPair<QString, TrackDto>> snapshot = snapshotTracks(tracks);

    QHash<QString, QVariant> changed;
    changed.insert(TrackEditModel::KeyArtist, newName);
    QList<TrackFileSystemDto> updatedFiles;
    auto [succeeded, failed] = saveMetadataBatchCore(m_libraryController->getTracksByArtist(id), changed, updatedFiles);

    if (!ArtistDao::rename(id, newName)) {
        emit batchOperationFinished("Rinomina artista", succeeded, failed + 1);
        emit errorOccurred("errore nell'aggiornamento dell'artista nel db");
        return;
    }

    emit batchOperationFinished("Rinomina artista", succeeded, failed);

    if (!updatedFiles.isEmpty() && !m_databaseController->updateNewTracks(updatedFiles)) {
        emit errorOccurred("errore nell'aggiornamento delle tracce nel db");
    }
    
    if (failed == 0) {
        QSet<QString> paths;
        for (const auto& s : snapshot) paths.insert(s.first);
        setLastAction(QString("Annulla: rinomina artista in \"%1\"").arg(oldName), paths,
            [this, id, oldName, snapshot]() {
                ArtistDao::rename(id, oldName);
                restoreTrackSnapshots(snapshot);
            });
    }

    emit libraryUpdated();
}

void AppController::requestRenameGenre(int id, const QString& newName) {
    auto genreOpt = m_libraryController->getGenreById(id);
    const QString oldName = genreOpt ? genreOpt->name() : QString();
    const QList<Track> tracks = m_libraryController->getTracksByGenre(id);
    const QList<QPair<QString, TrackDto>> snapshot = snapshotTracks(tracks);

    QHash<QString, QVariant> changed;
    changed.insert(TrackEditModel::KeyGenre, newName);

    QList<TrackFileSystemDto> updatedFiles;
    auto [succeeded, failed] = saveMetadataBatchCore(m_libraryController->getTracksByGenre(id), changed, updatedFiles);

    if (!GenreDao::rename(id, newName)) {
        emit batchOperationFinished("Rinomina genere", succeeded, failed + 1);
        emit errorOccurred("errore nell'aggiornamento del genere nel db");
        return;
    }

    emit batchOperationFinished("Rinomina genere", succeeded, failed);

    if (!updatedFiles.isEmpty() && !m_databaseController->updateNewTracks(updatedFiles)) {
        emit errorOccurred("errore nell'aggiornamento delle tracce nel db");
    }

    if (failed == 0) {
        QSet<QString> paths;
        for (const auto& s : snapshot) paths.insert(s.first);
        setLastAction(QString("Annulla: rinomina artista in \"%1\"").arg(oldName), paths,
            [this, id, oldName, snapshot]() {
                GenreDao::rename(id, oldName);
                restoreTrackSnapshots(snapshot);
            });
    }

    emit libraryUpdated();
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
    
    if (m_lastAction) {
        for (const QString& p : list) {
            if (m_lastAction->affectedPaths.contains(p)) { // se file eliminato presente in lastAction -> impedisci undo
                m_lastAction.reset();
                emit undoAvailabilityChanged(false, QString());
                break;
            }
        }
    }

    qDebug() << "Allineamento db";
    if(!m_databaseController->deleteNewTracks(list)) emit errorOccurred("Sincronizzazione db fallita");
    else emit libraryUpdated();

    if (!success) {
        qWarning() << "Impossibile eliminare:" << absolutePath;
    }
}

void AppController::requestCleanTagsBatch(const QList<QString>& relativePaths) {
    const QList<QPair<QString, TrackDto>> snapshot = snapshotTracks(m_libraryController->getTracksFromRelativePaths(relativePaths));
    int succeeded = 0, failed = 0;
    for (const QString& path : relativePaths) {
        if (cleanTagsCore(path).isEmpty()) ++succeeded; 
        else ++failed;
    }
    emit batchOperationFinished("Clean Tags", succeeded, failed);
    if (failed == 0 && !snapshot.isEmpty()) {
        QSet<QString> paths;
        for (const auto& s : snapshot) paths.insert(s.first);
        setLastAction("Annulla: modifica metadati batch", paths, [this, snapshot]() {
            restoreTrackSnapshots(snapshot);
        });
    }
    if (succeeded > 0) emit libraryUpdated();
}

void AppController::requestRemoveCoverBatch(const QList<QString>& relativePaths) {
    const QList<QPair<QString, TrackDto>> snapshot = snapshotTracks(m_libraryController->getTracksFromRelativePaths(relativePaths));

    int succeeded = 0, failed = 0;
    for (const QString& path : relativePaths) {
        if (removeCoverCore(path).isEmpty()) ++succeeded; 
        else ++failed;
    }
    emit batchOperationFinished("Rimuovi artwork", succeeded, failed);
    if (succeeded > 0) emit libraryUpdated();

    if (failed == 0 && !snapshot.isEmpty()) {
        QSet<QString> paths;
        for (const auto& s : snapshot) paths.insert(s.first);
        setLastAction("Annulla: modifica metadati batch", paths, [this, snapshot]() {
            restoreTrackSnapshots(snapshot);
        });
    }
}

QString AppController::saveMetadataFileCore(const QString& relativePath, const TrackDto& dto) {
    if (!m_metadataController->saveMetadata(relativePath, dto)) return "Errore nel salvataggio dei metadati di " + relativePath;
    return QString();
}

QPair<int, int> AppController::saveMetadataBatchCore(const QList<Track>& tracks, const QHash<QString, QVariant>& changedValues, QList<TrackFileSystemDto>& updatedFilesOut) {
    int succeeded = 0, failed = 0;

    for (const Track& t : tracks) {
        TrackEditModel model(t, m_libraryController);
        const TrackDto dto = model.buildDto(changedValues);

        if (saveMetadataFileCore(t.relativePath(), dto).isEmpty()) {
            QFileInfo info(m_storageController->resolveToAbsolutePath(t.relativePath()));
            updatedFilesOut.append({t.relativePath(), info.size(), info.lastModified().toSecsSinceEpoch()});
            ++succeeded;
        } else ++failed;
    }

    return {succeeded, failed};
}

QString AppController::setCoverCore(const QString& relativePath, const QString& imagePath) {
    QImage image(imagePath);
    if (image.isNull()) return "L'immagine è null";

    const QString artworkHash = ImageUtils::contentHash(image);
    if (artworkHash.isEmpty()) return "Hash è empty";

    const QString cachedPath = ImageUtils::cacheArtwork(image, m_storageController->artworkCacheDirectory());
    if (cachedPath.isEmpty()) return "cachedPath è empty";

    if (!m_metadataController->setCover(relativePath, imagePath)) return "errore nel set della cover nei metadata";

    std::optional<Track> t = TrackDao::findByRelativePath(relativePath);
    if (!t) return "Errore nel recupero della traccia " + relativePath;

    if (!TrackDao::updateCover(t->id(), cachedPath)) return "errore nell'update della cover nel db";

    return QString();
}

QString AppController::removeCoverCore(const QString& relativePath) {
    std::optional<Track> t = TrackDao::findByRelativePath(relativePath);
    if (!t) return "Errore nel recupero della traccia " + relativePath;

    if (!m_metadataController->removeCover(relativePath)) return "Errore nell'eliminazione della cover";
    if (!TrackDao::updateCover(t->id(), "NULL")) return "errore nell'update della cover nel db";

    return QString();
}

QString AppController::cleanTagsCore(const QString& relativePath) {
    std::optional<Track> t = TrackDao::findByRelativePath(relativePath);
    if (!t) return "Errore nel recupero della traccia " + relativePath;

    if (!m_metadataController->cleanTags(relativePath)) return "Errore nella pulizia dei tag del file";
    if (!TrackDao::cleanTags(t->id())) return "errore nell'update dei tag nel db";

    return QString();
}

QList<QPair<QString, TrackDto>> AppController::snapshotTracks(const QList<Track>& tracks) const {
    QList<QPair<QString, TrackDto>> snapshots;
    for (const Track& t : tracks) {
        TrackEditModel model(t, m_libraryController);
        snapshots.append({t.relativePath(), model.buildDto({})}); // hash vuoto = nessuna modifica, solo lo stato attuale
    }
    return snapshots;
}

void AppController::restoreTrackSnapshots(const QList<QPair<QString, TrackDto>>& snapshots) {
    QList<TrackFileSystemDto> updatedFiles;

    for (const auto& pair : snapshots) {
        const QString& path = pair.first;
        const TrackDto& dto = pair.second;

        saveMetadataFileCore(path, dto);

        if (!dto.coverHash.isEmpty() && dto.coverHash != "NULL") {
            auto t = TrackDao::findByRelativePath(path);
            if(t) TrackDao::updateCover(t->id(), dto.coverHash); // hash nudo, nessun ricalcolo/ricache
        } else {
            removeCoverCore(path);
        }

        QFileInfo info(m_storageController->resolveToAbsolutePath(path));
        updatedFiles.append({path, info.size(), info.lastModified().toSecsSinceEpoch()});
    }

    if (!updatedFiles.isEmpty()) m_databaseController->updateNewTracks(updatedFiles);
    emit libraryUpdated();
}

void AppController::setLastAction(const QString& description, const QSet<QString>& affectedPaths, std::function<void()> undo) {
    m_lastAction = UndoAction{description, affectedPaths, undo};
    emit undoAvailabilityChanged(true, description);
}

void AppController::requestUndoLastChange() {
    if (!m_lastAction) return;
    m_lastAction->undo();
    m_lastAction.reset();
    emit undoAvailabilityChanged(false, QString());
}

bool AppController::hasUndoableChange() const { return m_lastAction.has_value(); }

QString AppController::undoDescription() const { return m_lastAction ? m_lastAction->description : QString(); }