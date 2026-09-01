#include "utils/imageutils.h"

#include <QBuffer>
#include <QCryptographicHash>
#include <QDir>
#include <QFileInfo>
#include <QImageReader>
#include <QImageWriter>

QByteArray ImageUtils::normalizedPngData(const QImage& image) {
    if (image.isNull()) return {};
    const QImage normalized = image.convertToFormat(QImage::Format_RGBA8888);
    QByteArray data;
    QBuffer buffer(&data);

    if (!buffer.open(QIODevice::WriteOnly)) return {};

    QImageWriter writer(&buffer, "png");

    if (!writer.write(normalized)) return {};

    return data;
}

QString ImageUtils::contentHash(const QImage& image) {
    const QByteArray data = normalizedPngData(image);
    if (data.isEmpty()) return {};

    return QString::fromLatin1(
        QCryptographicHash::hash(
            data,
            QCryptographicHash::Sha256
        ).toHex()
    );
}

QString ImageUtils::contentHashFromFile(const QString& filePath) {
    QImage image;
    if (!image.load(filePath)) return {};

    return contentHash(image);
}

QImage ImageUtils::resizeThumbnail(const QImage& image, const QSize& maxSize) {
    if (image.isNull() || !maxSize.isValid()) return {};

    if (image.size().width() <= maxSize.width() &&
        image.size().height() <= maxSize.height()) {
        return image;
    }

    return image.scaled(
        maxSize,
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation
    );
}

QString ImageUtils::cacheArtwork(const QImage& image, const QString& artworkCacheDirectory, const QSize& thumbnailSize) {
    if (image.isNull() || artworkCacheDirectory.isEmpty()) return {};
    const QString hash = contentHash(image);
    if (hash.isEmpty()) return {};
    QDir directory;
    if (!directory.mkpath(artworkCacheDirectory)) return {};
    const QString filePath = QDir(artworkCacheDirectory).filePath(hash + QStringLiteral(".jpg"));

    if (QFileInfo::exists(filePath)) return filePath;

    const QImage thumbnail = resizeThumbnail(image, thumbnailSize);

    if (thumbnail.isNull()) return {};
    QImageWriter writer(filePath, "jpg");

    writer.setQuality(Constants::Artwork::JpegQuality);

    if (!writer.write(thumbnail)) return {};

    return filePath;
}

QString ImageUtils::cacheArtwork(const QString& imagePath, const QString& artworkCacheDirectory, const QSize& thumbnailSize) {
    QImage image;
    if (!image.load(imagePath)) return {};

    return cacheArtwork(
        image,
        artworkCacheDirectory,
        thumbnailSize
    );
}
