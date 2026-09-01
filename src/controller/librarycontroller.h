#ifndef LIBRARYCONTROLLER_H
#define LIBRARYCONTROLLER_H

#include <QObject>
#include <QList>
#include <QString>

#include "model/artist.h"
#include "model/album.h"
#include "model/track.h"
#include "model/genre.h"
#include "dto/searchresultdto.h"

class LibraryController : public QObject{
    Q_OBJECT
public:
    explicit LibraryController(QObject* parent = nullptr);

    QList<Artist> getAllArtists() const;
    std::optional<Artist> getArtistById(int artistId) const;
    QList<Artist> searchArtists(const QString& keyword) const;

    QList<Album> getAllAlbums() const;
    std::optional<Album> getAlbumById(int albumId) const;
    QList<Album> getAlbumsByArtist(int artistId) const;
    QList<Album> searchAlbums(const QString& keyword) const;
    int getAlbumDurationSeconds(int albumId) const;
    Album createAlbumFromTracks(const QList<Track>& tracks) const;
    QList<Album> getAlbumsForArtist(int artistId) const;
    QList<Track> getUnknownAlbumOfArtist(int artistId) const;

    QList<Track> getAllTracks() const;
    std::optional<Track> getTrackById(int trackId) const;
    std::optional<Track> getTrackByRelativePath(const QString& relativePath) const;
    
    QList<Track> getTracksByAlbum(int albumId) const;
    QList<Track> getTracksByArtist(int artistId) const;
    QList<Track> getTracksByGenre(int genreId) const;
    QList<Track> searchTracks(const QString& keyword) const;
    
    QList<Genre> getAllGenres() const;
    std::optional<Genre> getGenreById(int genreId) const;
    QList<Genre> searchGenres(const QString& keyword) const;

    LibrarySearchResult search(const QString& keyword) const;

    int countTracksByAlbum(int albumId) const;
    int countAlbumsByArtist(int artistId) const;
    int countTracksByArtist(int artistId) const;
    int countArtistsByGenre(int genreId) const;
    int countAlbumsByGenre(int genreId) const;
};
    
#endif // LIBRARYCONTROLLER_H