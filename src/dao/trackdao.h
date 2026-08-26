#ifndef TRACKDAO_H
#define TRACKDAO_H

#include "model/track.h"
#include "dto/trackfilesystemdto.h"

#include <optional>
#include <QList>

class TrackDao {
public:
    static bool insert(Track& track);
    static bool update(const Track& track);
    static bool deleteById(int id);
    static bool deleteByRelativePath(const QString& relativePath);
    
    static std::optional<Track> findById(int id);
    static QList<Track> getAll();
    static QList<Track> getByAlbumId(int albumId);
    static QList<Track> getByArtistId(int artistId);
    static QList<Track> getByGenreId(int genreId);
    static QList<Track> searchByKeyword(const QString& keyword);
    static QHash<QString, TrackFileSystemDto> getAllFileStates();
};

#endif // TRACKDAO_H