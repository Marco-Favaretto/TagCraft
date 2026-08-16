#ifndef TRACKFILESYSTEMDTO_H
#define TRACKFILESYSTEMDTO_H

#include <QString>

struct TrackFileSystemDto {
    QString absolutePath;
    qint64 fileSize;
    qint64 lastModified;
};

#endif // TRACKFILESYSTEMDTO_H