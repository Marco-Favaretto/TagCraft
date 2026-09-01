#include "librarycontroller.h"

#include "dao/artistdao.h"
#include "dao/albumdao.h"
#include "dao/trackdao.h"
#include "dao/genredao.h"
#include "dto/constants.h"

#include <QSet>

LibraryController::LibraryController(QObject* parent) : QObject(parent) {}

// Artists

QList<Artist> LibraryController::getAllArtists() const {
    return ArtistDao::getAll();
}

std::optional<Artist> LibraryController::getArtistById(int artistId) const {
    return ArtistDao::findById(artistId);
}

QList<Artist> LibraryController::searchArtists(const QString& keyword) const {
    return ArtistDao::searchByKeyword(keyword);
}

// Albums

QList<Album> LibraryController::getAllAlbums() const {
    return AlbumDao::getAll();
}

std::optional<Album> LibraryController::getAlbumById(int albumId) const {
    return AlbumDao::findById(albumId);
}

QList<Album> LibraryController::getAlbumsByArtist(int artistId) const {
    return AlbumDao::getByArtistId(artistId);
}

QList<Album> LibraryController::searchAlbums(const QString& keyword) const {
    return AlbumDao::searchByKeyword(keyword);
}

int LibraryController::getAlbumDurationSeconds(int albumId) const {
    const auto tracks = TrackDao::getByAlbumId(albumId);
    int total = 0;
    for (const auto& t : tracks) {
        if (t.durationSeconds()) total += *t.durationSeconds();
    }
    return total;
}

QList<Album> LibraryController::getAlbumsForArtist(int artistId) const {
    QList<Album> albums = AlbumDao::getByArtistId(artistId);

    QList<Track> unknownTracks = TrackDao::getUnknownAlbumOfArtist(artistId);
    if (!unknownTracks.isEmpty()) {
        albums.append(createAlbumFromTracks(unknownTracks));
    }

    return albums;
}

Album LibraryController::createAlbumFromTracks(const QList<Track>& tracks) const {
    Album album;
    album.setId(Constants::DefaultValues::AlbumId);
    album.setTitle(Constants::DefaultValues::Album);
    album.setArtistId(tracks.isEmpty() ? Constants::DefaultValues::ArtistId : tracks.first().artistId());

    std::optional<int> year = tracks.isEmpty() ? std::nullopt : tracks.first().year();
    for (const auto& t : tracks) {
        if (t.year() != year) { year = std::nullopt; break; }
    }
    album.setYear(year);

    return album;
}


QList<Track> LibraryController::getUnknownAlbumOfArtist(int artistId) const {
    return TrackDao::getUnknownAlbumOfArtist(artistId);
}

// Tracks

QList<Track> LibraryController::getAllTracks() const {
    return TrackDao::getAll();
}

std::optional<Track> LibraryController::getTrackById(int trackId) const {
    return TrackDao::findById(trackId);
}

std::optional<Track> LibraryController::getTrackByRelativePath(
    const QString& relativePath) const {
    return TrackDao::findByRelativePath(relativePath);
}

QList<Track> LibraryController::getTracksByAlbum(int albumId) const {
    return TrackDao::getByAlbumId(albumId);
}

QList<Track> LibraryController::getTracksByArtist(int artistId) const {
    return TrackDao::getByArtistId(artistId);
}

QList<Track> LibraryController::getTracksByGenre(int genreId) const {
    return TrackDao::getByGenreId(genreId);
}

QList<Track> LibraryController::searchTracks(const QString& keyword) const {
    return TrackDao::searchByKeyword(keyword);
}

// genre

QList<Genre> LibraryController::getAllGenres() const {
    return GenreDao::getAll();
}

std::optional<Genre> LibraryController::getGenreById(int genreId) const {
    return GenreDao::findById(genreId);
}

QList<Genre> LibraryController::searchGenres(const QString& keyword) const {
    return GenreDao::searchByKeyword(keyword);
}

// global

LibrarySearchResult LibraryController::search(const QString& keyword) const {
    LibrarySearchResult result;

    result.artists = ArtistDao::searchByKeyword(keyword);
    result.albums  = AlbumDao::searchByKeyword(keyword);
    result.tracks  = TrackDao::searchByKeyword(keyword);
    result.genres  = GenreDao::searchByKeyword(keyword);

    return result;
}

int LibraryController::countTracksByAlbum(int albumId) const {
    return TrackDao::getByAlbumId(albumId).size();
}

int LibraryController::countAlbumsByArtist(int artistId) const {
    return AlbumDao::getByArtistId(artistId).size();
}

int LibraryController::countTracksByArtist(int artistId) const {
    return TrackDao::getByArtistId(artistId).size();
}

int LibraryController::countArtistsByGenre(int genreId) const {
    const auto tracks = TrackDao::getByGenreId(genreId);
    QSet<int> artistIds;
    for (const auto& t : tracks) artistIds.insert(t.artistId());
    return artistIds.size();
}

int LibraryController::countAlbumsByGenre(int genreId) const {
    const auto tracks = TrackDao::getByGenreId(genreId);
    QSet<int> albumIds;
    for (const auto& t : tracks) albumIds.insert(t.albumId());
    return albumIds.size();
}