#include "entitymapper.h"
#include "utils/dbutils.h"

Track EntityMapper::toEntityTrack(const QSqlQuery& query) {
    Track track;
    
    track.setId(query.value("id").toInt());
    track.setTitle(query.value("title").toString());
    track.setArtistId(query.value("artist_id").toInt());
    track.setAlbumId(query.value("album_id").toInt());
    track.setRelativePath(query.value("relative_path").toString());
    track.setFileMtimeSecs(query.value("file_mtime").toLongLong());
    track.setFileSize(query.value("file_size").toLongLong());

    track.setGenreId(DbUtils::variantToOptional<int>(query.value("genre_id")));
    track.setYear(DbUtils::variantToOptional<int>(query.value("year")));
    track.setTrackNumber(DbUtils::variantToOptional<int>(query.value("track_number")));
    track.setDurationSeconds(DbUtils::variantToOptional<int>(query.value("duration_seconds")));
    track.setTrackCoverHash(DbUtils::variantToOptional<QString>(query.value("track_cover_hash")));

    return track;
}

Album EntityMapper::toEntityAlbum(const QSqlQuery& query) {
    Album album;

    album.setId(query.value("id").toInt());
    album.setTitle(query.value("title").toString());
    album.setArtistId(query.value("artist_id").toInt());
    album.setYear(DbUtils::variantToOptional<int>(query.value("year")));
    album.setCoverCacheHash(
        DbUtils::variantToOptional<QString>(query.value("cover_cache_hash"))
    );

    return album;
}

Artist EntityMapper::toEntityArtist(const QSqlQuery& query) {
    Artist artist;

    artist.setId(query.value("id").toInt());
    artist.setName(query.value("name").toString());

    return artist;
}

Genre EntityMapper::toEntityGenre(const QSqlQuery& query) {
    Genre genre;

    genre.setId(query.value("id").toInt());
    genre.setName(query.value("name").toString());

    return genre;
}

TrackFileSystemDto EntityMapper::trackToTFSDto(const Track& track) {
    TrackFileSystemDto dto;

    dto.relativePath = track.relativePath();
    dto.fileSize = track.fileSize();
    dto.lastModified = track.fileMtimeSecs();

    return dto;
}

TrackFileSystemDto EntityMapper::trackDtoToTFSDto(const TrackDto& trackDto) {
    TrackFileSystemDto dto;

    dto.relativePath = trackDto.relativePath;
    dto.fileSize = trackDto.fileSize;
    dto.lastModified = trackDto.lastModified;

    return dto;
}