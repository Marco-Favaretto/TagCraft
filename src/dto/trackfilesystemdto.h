#ifndef TRACKFILESYSTEMDTO_H
#define TRACKFILESYSTEMDTO_H

#include <QString>

// da usare per scan veloce
struct TrackFileSystemDto {
    QString relativePath;
    qint64 fileSize;
    qint64 lastModified;
};

#endif // TRACKFILESYSTEMDTO_H