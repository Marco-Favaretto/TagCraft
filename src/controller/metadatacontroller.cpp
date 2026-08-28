#include "controller/metadatacontroller.h"

#include "storage/storagemanager.h"

#include "utils/tagmapper.h"
#include "utils/imageutils.h"

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

QByteArray MetadataController::resolveArtwork(const QString& relativePath) const {
    QString absolutePath = StorageManager::instance().toAbsolutePath(relativePath);

    // 1. APIC embedded nel file
    QByteArray embedded = TagMapper::extractEmbeddedCover(absolutePath);
    if (!embedded.isEmpty()) return embedded;

    // 2. cover.jpg nella cartella dell'album
    QString albumDir = QFileInfo(absolutePath).absolutePath();
    QString coverPath = QDir(albumDir).filePath("cover.jpg");
    if (QFileInfo::exists(coverPath)) {
        QFile file(coverPath);
        if (file.open(QIODevice::ReadOnly)) return file.readAll();
    }

    // 3. placeholder applicativo (nessun dato — la UI userà l'icona di default)
    return QByteArray();
}

QString MetadataController::resolveAndCacheArtwork(const QString& relativePath) {
    QByteArray data = resolveArtwork(relativePath);
    if (data.isEmpty()) return {};

    QImage image;
    image.loadFromData(data);

    QString hash = ImageUtils::contentHash(image);
    if (hash.isEmpty()) return {};

    ImageUtils::cacheArtwork(image, StorageManager::instance().artworkCacheDirectory());

    return hash;
}