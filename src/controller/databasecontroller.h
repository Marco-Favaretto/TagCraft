#ifndef DATABASECONTROLLER_H
#define DATABASECONTROLLER_H

#include <QObject>
#include <QList>
#include <QString>

#include "dto/scanresultdto.h"
#include "dto/trackfilesystemdto.h"

#include "model/album.h"
#include "model/artist.h"
#include "model/genre.h"
#include "model/track.h"

class DatabaseController : public QObject {
    Q_OBJECT
public:
    explicit DatabaseController(QObject* parent = nullptr);

    bool insertNewTracks(const QList<TrackFileSystemDto>&);
    bool updateNewTracks(const QList<TrackFileSystemDto>&);
    bool deleteNewTracks(const QList<QString>&);
    bool deleteOrphans();

    bool resetDb();

private:
    std::optional<Track> insertTrackInternal(const TrackFileSystemDto&);
    bool updateTrackInternal(const TrackFileSystemDto&);
};

#endif // DATABASECONTROLLER_H