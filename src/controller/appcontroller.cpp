#include "controller/appcontroller.h"

#include "db/databasemanager.h"
#include "db/entitymapper.h"
#include "storage/storagemanager.h"
#include "dao/trackdao.h"
#include "utils/imageutils.h"

#include <QImage>

AppController::AppController(QObject* parent) : QObject(parent) {}

bool AppController::initialize() {
    if (!StorageManager::instance().scanForStorage()) {
        emit errorOccurred("Storage esterno non trovato");
    }

    QString dbPath = StorageManager::instance().musicAppPoint() + "/music_library.db";
    if (!DatabaseManager::instance().openDatabase(dbPath)) {
        emit errorOccurred("Impossibile aprire il database");
        return false;
    }
    DatabaseManager::instance().initSchema();

    m_storageController = new StorageController(this);
    m_metadataController = new MetadataController(this);
    m_databaseController = new DatabaseController(this);

    setupConnections();

    emit appReady();
    return true;
}

void AppController::setupConnections() {
    connect(m_storageController, &StorageController::scanFinished,
            this, &AppController::onFullScanFinished);
    connect(m_storageController, &StorageController::fullScanFinished,
            this, &AppController::onScanFinished);
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
}

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
    if (!result.newTracks.isEmpty() && !m_databaseController->insertNewTracks(result.newTracks))
        emit errorOccurred("Inserimento nuove tracce fallito");
    if (!result.modifiedTracks.isEmpty() && !m_databaseController->updateNewTracks(result.modifiedTracks))
        emit errorOccurred("Aggiornamento tracce fallito");
    if (!result.deletedTracks.isEmpty() && !m_databaseController->deleteNewTracks(result.deletedTracks))
        emit errorOccurred("Eliminazione tracce fallito");
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
    if (!m_databaseController->insertNewTracks(list))
        emit errorOccurred("Ricostruzione database fallita");
    else
        emit libraryUpdated();
}