#ifndef SEARCHRESULTDTO_H
#define SEARCHRESULTDTO_H

#include "model/album.h"
#include "model/artist.h"
#include "model/genre.h"
#include "model/track.h"

#include <QList>

struct LibrarySearchResult {
    QList<Artist> artists;
    QList<Album> albums;
    QList<Track> tracks;
    QList<Genre> genres;
};

#endif // SEARCHRESULTDTO_H