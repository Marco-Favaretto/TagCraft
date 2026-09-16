#ifndef TRACKDTO_H
#define TRACKDTO_H

#include <QString>
#include <QDateTime>

struct TrackDto {
    int id;
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
            "TrackDto{id='%1', title='%2', artist='%3', album='%4', genre='%5', "
            "track=%6, year=%7, duration=%8s, size=%9B, "
            "modified='%10', path='%11', coverHash='%12'}"
        )
        .arg(id)
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