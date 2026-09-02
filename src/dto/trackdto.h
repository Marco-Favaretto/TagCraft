#ifndef TRACKDTO_H
#define TRACKDTO_H

#include <QString>
#include <QDateTime>

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
    QString coverHash = "NULL";

    QString toString() const {
        const auto modified =
            QDateTime::fromMSecsSinceEpoch(lastModified)
                .toString(Qt::ISODate);

        return QString(
            "TrackDto{title='%1', artist='%2', album='%3', genre='%4', "
            "track=%5, year=%6, duration=%7s, size=%8B, "
            "modified='%9', path='%10', coverHash='%11'}"
        )
        .arg(title)
        .arg(artistName)
        .arg(albumName)
        .arg(genreName)
        .arg(trackNumber)
        .arg(year)
        .arg(durationSeconds)
        .arg(fileSize)
        .arg(modified)
        .arg(relativePath)
        .arg(coverHash);
    }
};  

#endif // TRACKDTO_H