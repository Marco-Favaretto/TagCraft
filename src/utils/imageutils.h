#ifndef IMAGEUTILS_H
#define IMAGEUTILS_H

#include <QImage>
#include <QString>
#include <QSize>

class ImageUtils {
public:
    ImageUtils() = delete;

    // Calcola hash SHA-256 del contenuto dell'immagine.
    static QString contentHash(const QImage& image);

    // Carica immagine da file e ne calcola l'hash SHA-256.
    static QString contentHashFromFile(const QString& filePath);

    // Ridimensiona immagine mantenendo le proporzioni senza ingrandirla.
    static QImage resizeThumbnail(const QImage& image, const QSize& maxSize);

    // Ridimensiona l'immagine e salva la thumbnail nella cache .music_app/covers/.
    static QString cacheArtwork(const QImage& image, const QString& musicAppDirectory, const QSize& thumbnailSize = QSize(300, 300));

    // Carica un'immagine da file, la ridimensiona e la salva nella cache.
    static QString cacheArtwork(const QString& imagePath, const QString& musicAppDirectory, const QSize& thumbnailSize = QSize(300, 300));

private:
    // Converte l'immagine in un formato PNG normalizzato per ottenere un hash stabile.
    static QByteArray normalizedPngData(const QImage& image);
};

#endif // IMAGEUTILS_H