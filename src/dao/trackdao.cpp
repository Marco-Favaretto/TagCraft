#include "trackdao.h"
#include "db/sqlparser.h"
#include "db/sqlexecutor.h"
#include "utils/dbutils.h"
#include "db/entitymapper.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QFile>
#include <QTextStream>
#include <QMap>
#include <QDebug>

bool TrackDao::insert(Track& track) {
    static const auto queries = SqlParser::parseNamedQueries(":/sql/track.sql");
    const QString queryString = queries.value("insert");

    if (queryString.isEmpty()) {
        qCritical() << "Query 'insert' non trovata in track.sql";
        return false;
    }

    QSqlQuery query;
    query.prepare(queryString);

    SqlExecutor::execute(query,{
        {":title", track.title()},
        {":artist_id", track.artistId()},
        {":album_id", track.albumId()},
        {":genre_id", DbUtils::optionalToVariant(track.genreId())},
        {":year", DbUtils::optionalToVariant(track.year())},
        {":track_number", DbUtils::optionalToVariant(track.trackNumber())},
        {":duration_seconds", DbUtils::optionalToVariant(track.durationSeconds())},
        {":relative_path", track.relativePath()},
        {":file_mtime", track.fileMtimeSecs()},
        {":file_size", track.fileSize()},
        {":track_cover_hash", DbUtils::optionalToVariant(track.trackCoverHash())}
    });

    // id generato da autoincrement
    track.setId(query.lastInsertId().toInt());

    return true;
}

bool TrackDao::update(const Track& track) {
    static const auto queries = SqlParser::parseNamedQueries(":/sql/track.sql");
    const QString queryString = queries.value("update");

    if (queryString.isEmpty()) {
        qCritical() << "Query 'update' non trovata in track.sql";
        return false;
    }

    QSqlQuery query;
    query.prepare(queryString);

    return SqlExecutor::execute(query, {
        {":id", track.id()},
        {":title", track.title()},
        {":artist_id", track.artistId()},
        {":album_id", track.albumId()},
        {":genre_id", DbUtils::optionalToVariant(track.genreId())},
        {":year", DbUtils::optionalToVariant(track.year())},
        {":track_number", DbUtils::optionalToVariant(track.trackNumber())},
        {":duration_seconds", DbUtils::optionalToVariant(track.durationSeconds())},
        {":relative_path", track.relativePath()},
        {":file_mtime", track.fileMtimeSecs()},
        {":file_size", track.fileSize()},
        {":track_cover_hash", DbUtils::optionalToVariant(track.trackCoverHash())}
    });
}


bool TrackDao::updateCover(int id, const QString& imagehash) {
    static const auto queries = SqlParser::parseNamedQueries(":/sql/track.sql");
    const QString queryString = queries.value("updateCover");

    if (queryString.isEmpty()) {
        qCritical() << "Query 'updateCover' non trovata in track.sql";
        return false;
    }

    QSqlQuery query;
    query.prepare(queryString);

    return SqlExecutor::execute(query, {
        {":id", id},
        {":track_cover_hash", imagehash}
    });
}

std::optional<Track> TrackDao::findById(int id) {
    static const auto queries = SqlParser::parseNamedQueries(":/sql/track.sql");
    const QString queryString = queries.value("findById");

    if (queryString.isEmpty()) {
        qCritical() << "Query 'findById' non trovata in track.sql";
        return std::nullopt;
    }

    QSqlQuery query;
    query.prepare(queryString);
    query.bindValue(":id", id);

    if (SqlExecutor::execute(query, { {":id", id} })) {
        if (query.next()) {
            return EntityMapper::toEntityTrack(query);
        }
    }

    return std::nullopt;
}

std::optional<Track> TrackDao::findByRelativePath(const QString& relativePath) {
    static const auto queries = SqlParser::parseNamedQueries(":/sql/track.sql");
    const QString queryString = queries.value("findByRelativePath");

    if (queryString.isEmpty()) {
        qCritical() << "Query 'findByRelativePath' non trovata in track.sql";
        return std::nullopt;
    }

    QSqlQuery query;
    query.prepare(queryString);
    query.bindValue(":relative_path", relativePath);

    if (SqlExecutor::execute(query, { {":relative_path", relativePath} })) {
        if (query.next()) {
            return EntityMapper::toEntityTrack(query);
        }
    }

    return std::nullopt;
}

QList<Track> TrackDao::getAll() {
    QList<Track> tracks;
    static const auto queries = SqlParser::parseNamedQueries(":/sql/track.sql");
    const QString queryString = queries.value("getAll");

    if (queryString.isEmpty()) {
        qCritical() << "Query 'getAll' non trovata in track.sql";
        return tracks;
    }

    QSqlQuery query(queryString);

    if(SqlExecutor::execute(query, {})) {
        while (query.next()) {
            tracks.append(EntityMapper::toEntityTrack(query));
        }
    }
    return tracks;
}

QHash<QString, TrackFileSystemDto> TrackDao::getAllFileStates() {
    QHash<QString, TrackFileSystemDto> states;
    static const auto queries = SqlParser::parseNamedQueries(":/sql/track.sql");
    const QString queryString = queries.value("filesState");
    if (queryString.isEmpty()) {
        qCritical() << "Query 'filesState' non trovata in track.sql";
        return states;
    }

    QSqlQuery query(queryString);

    if(SqlExecutor::execute(query, {})) {
        while (query.next()) {
            QString relativePath = query.value(0).toString();
            TrackFileSystemDto dto;
            dto.relativePath = relativePath;
            dto.lastModified = query.value(1).toLongLong();
            dto.fileSize = query.value(2).toLongLong();

            states.insert(relativePath, dto);
        }
    }

    return states;
}

bool TrackDao::deleteById(int id) {
    static const auto queries = SqlParser::parseNamedQueries(":/sql/track.sql");
    const QString queryString = queries.value("deleteById");
    if (queryString.isEmpty()) return false;

    QSqlQuery query;
    query.prepare(queryString);
    return SqlExecutor::execute(query, {{":id", id}});
}

bool TrackDao::deleteByRelativePath(const QString& relativePath) {
    static const auto queries = SqlParser::parseNamedQueries(":/sql/track.sql");
    const QString queryString = queries.value("deleteByRelativePath");
    if (queryString.isEmpty()) return false;

    QSqlQuery query;
    query.prepare(queryString);
    return SqlExecutor::execute(query, {{":relative_path", relativePath}});
}

QList<Track> TrackDao::getByAlbumId(int albumId) {
    QList<Track> tracks;
    static const auto queries = SqlParser::parseNamedQueries(":/sql/track.sql");
    const QString queryString = queries.value("getByAlbumId");
    if (queryString.isEmpty()) return tracks;

    QSqlQuery query;
    query.prepare(queryString);
    if (SqlExecutor::execute(query, {{":album_id", albumId}})) {
        while (query.next()) {
            tracks.append(EntityMapper::toEntityTrack(query));
        }
    }
    return tracks;
}

QList<Track> TrackDao::getByArtistId(int artistId) {
    QList<Track> tracks;
    static const auto queries = SqlParser::parseNamedQueries(":/sql/track.sql");
    const QString queryString = queries.value("getByArtistId");
    if (queryString.isEmpty()) return tracks;

    QSqlQuery query;
    query.prepare(queryString);
    if (SqlExecutor::execute(query, {{":artist_id", artistId}})) {
        while (query.next()) {
            tracks.append(EntityMapper::toEntityTrack(query));
        }
    }
    return tracks;
}

QList<Track> TrackDao::getByGenreId(int genreId) {
    QList<Track> tracks;
    static const auto queries = SqlParser::parseNamedQueries(":/sql/track.sql");
    const QString queryString = queries.value("getByGenreId");
    if (queryString.isEmpty()) return tracks;

    QSqlQuery query;
    query.prepare(queryString);
    if (SqlExecutor::execute(query, {{":genre_id", genreId}})) {
        while (query.next()) {
            tracks.append(EntityMapper::toEntityTrack(query));
        }
    }
    return tracks;
}

QList<Track> TrackDao::searchByKeyword(const QString& keyword) {
    QList<Track> tracks;
    static const auto queries = SqlParser::parseNamedQueries(":/sql/track.sql");
    const QString queryString = queries.value("searchByKeyword");
    if (queryString.isEmpty()) return tracks;

    QSqlQuery query;
    query.prepare(queryString);
    QString pattern = "%" + keyword.trimmed() + "%";
    if (SqlExecutor::execute(query, {{":keyword", pattern}})) {
        while (query.next()) {
            tracks.append(EntityMapper::toEntityTrack(query));
        }
    }
    return tracks;
}

bool TrackDao::drop() {
    static const auto queries = SqlParser::parseNamedQueries(":/sql/track.sql");
    const QString queryString = queries.value("drop");
    if (queryString.isEmpty()) return false;

    QSqlQuery query;
    query.prepare(queryString);
    return SqlExecutor::execute(query, {});
}