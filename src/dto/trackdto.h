#ifndef TRACKDTO_H
#define TRACKDTO_H

#include <QString>

struct TrackDto {
    QString relativePath;
    QString title;
    QString artistName;
    QString albumName;
    QString genreName;
    int trackNumber;
    int year;
    int durationSeconds;
    qint64 fileSize;
    qint64 lastModified;
};  

#endif // TRACKDTO_H