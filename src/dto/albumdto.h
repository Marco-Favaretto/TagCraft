#ifndef ALBUMDTO_H
#define ALBUMDTO_H

#include <QString>
#include <QDateTime>

struct AlbumDto {
    int id;
    QString relativePath;
    QString title;
    QString artistName;
    QString genreName;
    int year;
    QString coverHash = "NULL";

    QString toString() const {
        return QString(
            "AlbumDto{id='%1', title='%2', artist='%3', genre='%4', year=%5, path='%6', coverHash='%7'}"
        )
        .arg(id)
        .arg(title)
        .arg(artistName)
        .arg(genreName)
        .arg(year)
        .arg(relativePath)
        .arg(coverHash);
    }
};  

#endif // ALBUMDTO_H