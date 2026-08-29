#include "librarycontroller.h"

#include "dao/artistdao.h"
#include "dao/albumdao.h"
#include "dao/trackdao.h"
#include "dao/genredao.h"

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