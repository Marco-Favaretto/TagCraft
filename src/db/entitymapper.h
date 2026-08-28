#ifndef ENTITYMAPPER_H
#define ENTITYMAPPER_H

#include "model/track.h"
#include "model/album.h"
#include "model/artist.h"
#include "model/genre.h"
#include "dto/trackfilesystemdto.h"
#include "dto/trackdto.h"

#include <QSqlQuery>

class EntityMapper {
public:
    EntityMapper() = delete;

    static Track toEntityTrack(const QSqlQuery&);
    static Artist toEntityArtist(const QSqlQuery&);
    static Album toEntityAlbum(const QSqlQuery&);
    static Genre toEntityGenre(const QSqlQuery&);
    static TrackFileSystemDto trackToTFSDto(const Track& track);
    static TrackFileSystemDto trackDtoToTFSDto(const TrackDto& trackDto);
};

#endif // ENTITYMAPPER_H