#ifndef TRACKDAO_H
#define TRACKDAO_H

#include "model/track.h"
#include "dto/trackfilesystemdto.h"
#include "dto/firsttrackcovers.h"

#include <optional>
#include <QList>

class TrackDao {
public:
    static bool insert(Track& track);
    static bool update(const Track& track);
    static bool updateCover(int id, const QString& imagehash);
    static bool deleteById(int id);
    static bool deleteByRelativePath(const QString& relativePath);
    static bool drop();
    
    static std::optional<Track> findById(int id);
    static std::optional<Track> findByRelativePath(const QString&);
    static QList<Track> getTracksByAlbumRPath(const QString& albumRelativePath);
    static QList<QString> getRPathTracksFromAlbumRPath(const QString& albumRelativePath);
    static QList<Track> getAll();
    static QList<Track> getByAlbumId(int albumId);
    static QList<Track> getByArtistId(int artistId);
    static QList<Track> getByGenreId(int genreId);
    static QList<Track> searchByKeyword(const QString& keyword);
    static QHash<QString, TrackFileSystemDto> getAllFileStates();
    static QList<Track> getUnknownAlbumOfArtist(int artistId);

    static QList<FirstTrackCovers> getFirstTrackCovers();
};

#endif // TRACKDAO_H