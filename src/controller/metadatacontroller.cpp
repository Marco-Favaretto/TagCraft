#include "controller/metadatacontroller.h"

#include "storage/storagemanager.h"

#include "utils/tagmapper.h"

MetadataController::MetadataController(QObject* parent) : QObject(parent) {}

std::optional<TrackDto> MetadataController::readTags(const QString& relativePath) const {
    return TagMapper::fileToDto(StorageManager::instance().toAbsolutePath(relativePath), relativePath);
}

bool MetadataController::saveMetadata(const QString& relativePath, const TrackDto& newValues) {
    return TagMapper::dtoToFile(StorageManager::instance().toAbsolutePath(relativePath), newValues);
}


QByteArray MetadataController::extractCover(const QString& relativePath) const {
    return TagMapper::extractEmbeddedCover(StorageManager::instance().toAbsolutePath(relativePath));
}

bool MetadataController::setCover(const QString& relativePath, const QString& imagePath) {
    return TagMapper::embedCover(StorageManager::instance().toAbsolutePath(relativePath), imagePath);
}

bool MetadataController::cleanTags(const QString& relativePath) {
    return TagMapper::cleanTags(StorageManager::instance().toAbsolutePath(relativePath));
}

