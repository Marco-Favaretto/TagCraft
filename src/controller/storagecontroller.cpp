#include "controller/storagecontroller.h"

#include "storage/libraryscanner.h"
#include <QDir>
#include <QFileInfo>

StorageController::StorageController(QObject* parent) 
    : QObject(parent), 
      storage(StorageManager::instance()) 
{
    setupConnections();
}

// ScanResultDto StorageController::runScan(const QString& path) {
//     if (!validateMusicDirectory(path)) {
//         emit errorOccurred("Percorso libreria non valido o non accessibile: " + path);
//         return ScanResultDto{};
//     }

//     ScanResultDto result = LibraryScanner::smartScan(path);
//     emit scanFinished(result);
//     return result;
// }

ScanResultDto StorageController::runScan(const QString& path) {
    if (!validateMusicDirectory(path)) {
        emit errorOccurred("Percorso libreria non valido o non accessibile: " + path);
        return ScanResultDto{};
    }

    // Lambda per calcolare la percentuale e rilanciare il segnale
    // viene poi passata a smartScan che la esegue
    auto progressHandler = [this](int current, int total) {
        int percentage = (total > 0) ? static_cast<int>((current * 100.0) / total) : 0;
        emit scanProgress(percentage);
    };

    ScanResultDto result = LibraryScanner::smartScan(path, progressHandler);
    emit scanFinished(result);
    return result;
}

QList<TrackFileSystemDto> StorageController::runFullScan(const QString& path) {
    QList<TrackFileSystemDto> list;

    if (!validateMusicDirectory(path)) {
        emit errorOccurred("Percorso libreria non valido o non accessibile: " + path);
        return list;
    }

    // Lambda per calcolare la percentuale e rilanciare il segnale
    // viene poi passata a smartScan che la esegue
    auto progressHandler = [this](int current, int total) {
        int percentage = (total > 0) ? static_cast<int>((current * 100.0) / total) : 0;
        emit scanProgress(percentage);
    };

    list = LibraryScanner::scanAudioFiles(path, progressHandler);
    emit fullScanFinished(list);
    return list;
}

bool StorageController::isStorageMounted() const {
    return storage.isMounted();
}

QString StorageController::currentMountPoint() const {
    return storage.mountPoint();
}

QString StorageController::resolveToAbsolutePath(const QString& relativePath) const {
    return storage.toAbsolutePath(relativePath);
}

QString StorageController::resolveToRelativePath(const QString& absolutePath) const {
    return storage.toRelativePath(absolutePath);
}

bool StorageController::validateMusicDirectory(const QString& path) const {
    if (path.isEmpty()) return false;
    
    QFileInfo info(path);
    return info.exists() && info.isDir() && info.isReadable();
}

void StorageController::setupConnections() {
    connect(&storage, &StorageManager::storageMounted, this, &StorageController::storageMounted);
    connect(&storage, &StorageManager::storageUnmounted, this, &StorageController::storageUnmounted);
}