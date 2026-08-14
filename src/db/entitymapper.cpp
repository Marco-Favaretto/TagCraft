#include "entitymapper.h"
#include "utils/dbutils.h"

Track EntityMapper::toEntityTrack(const QSqlQuery& query) {
    Track track;
    
    track.setId(query.value("id").toInt());
    track.setTitle(query.value("title").toString());
    track.setArtistId(query.value("artist_id").toInt());
    track.setAlbumId(query.value("album_id").toInt());
    track.setRelativePath(query.value("relative_path").toString());
    track.setFileMtime(query.value("file_mtime").toLongLong());
    track.setFileSize(query.value("file_size").toLongLong());

    track.setGenreId(DbUtils::variantToOptional<int>(query.value("genre_id")));
    track.setYear(DbUtils::variantToOptional<int>(query.value("year")));
    track.setTrackNumber(DbUtils::variantToOptional<int>(query.value("track_number")));
    track.setDurationSeconds(DbUtils::variantToOptional<int>(query.value("duration_seconds")));
    track.setTrackCoverHash(DbUtils::variantToOptional<QString>(query.value("track_cover_hash")));

    return track;
}