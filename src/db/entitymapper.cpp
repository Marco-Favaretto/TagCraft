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
    album.setGenreId(query.value("genre_id").toInt());
    album.setYear(DbUtils::variantToOptional<int>(query.value("year")));
    album.setRelativePath(query.value("relative_path").toString());
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

FirstTrackCovers EntityMapper::toDtoFirstTrackCover(const QSqlQuery& query) {
    FirstTrackCovers ftc;

    ftc.albumId = query.value("album_id").toInt();
    ftc.hashCoverFirstTrack = query.value("track_cover_hash").toString();

    return ftc;
}

TrackDto EntityMapper::toTrackDto(const QSqlQuery& query) {
    TrackDto dto;

    dto.id = query.value("track.id").toInt();;
    dto.relativePath = query.value("track.relative_path").toString();
    dto.title = query.value("track.title").toString();
    dto.artistName = query.value("artist.name").toString();
    dto.albumName = query.value("album.title").toString();
    dto.genreName = query.value("genre.name").toString();
    
    std::optional<int> tmpOpt = DbUtils::variantToOptional<int>(query.value("track.track_number"));
    if(tmpOpt) dto.trackNumber = *tmpOpt;
    
    tmpOpt = DbUtils::variantToOptional<int>(query.value("track.year"));
    if(tmpOpt) dto.year = *tmpOpt;
    
    dto.durationSeconds = query.value("track.file_mtime").toLongLong();
    dto.fileSize = query.value("track.file_size").toLongLong();
    
    std::optional<QString> tmpOptHash = DbUtils::variantToOptional<QString>(query.value("track.track_cover_hash"));
    if(tmpOptHash) dto.coverHash = *tmpOptHash; 
    
    return dto;
}

AlbumDto EntityMapper::toAlbumDto(const QSqlQuery& query) {
    AlbumDto dto;

    dto.id = query.value("album.id").toInt();;
    dto.title = query.value("album.title").toString();
    dto.artistName = query.value("artist.name").toString();
    dto.genreName = query.value("genre.name").toString();
    
    std::optional<int> tmpOpt = DbUtils::variantToOptional<int>(query.value("album.year"));
    if(tmpOpt) dto.year = *tmpOpt;

    dto.relativePath = query.value("album.relative_path").toString();
    
    std::optional<QString> tmpOptHash = DbUtils::variantToOptional<QString>(query.value("album.cover_cache_hash"));
    if(tmpOptHash) dto.coverHash = *tmpOptHash; 
    
    return dto;
}