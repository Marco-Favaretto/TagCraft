#include "trackdao.h"
#include "utils/sqlparser.h"
#include "utils/dbutils.h"
#include "utils/entitymapper.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QFile>
#include <QTextStream>
#include <QMap>
#include <QDebug>

bool TrackDao::insert(Track &track) {
    static const auto queries = SqlParser::parseNamedQueries(":/sql/track.sql");
    const QString queryString = queries.value("insert");

    if (queryString.isEmpty()) {
        qCritical() << "Query 'insert' non trovata in track.sql";
        return false;
    }
    qDebug() << "Preparazione esecuzione query: " << queryString;

    QSqlQuery query;
    query.prepare(queryString);

    query.bindValue(":title", track.title());
    query.bindValue(":artist_id", track.artistId());
    query.bindValue(":album_id", track.albumId());
    query.bindValue(":genre_id", DbUtils::optionalToVariant(track.genreId()));
    query.bindValue(":year", DbUtils::optionalToVariant(track.year()));
    query.bindValue(":track_number", DbUtils::optionalToVariant(track.trackNumber()));
    query.bindValue(":duration_seconds", DbUtils::optionalToVariant(track.durationSeconds()));
    query.bindValue(":relative_path", track.relativePath());
    query.bindValue(":file_mtime", track.fileMtime());
    query.bindValue(":file_size", track.fileSize());
    query.bindValue(":track_cover_hash", DbUtils::optionalToVariant(track.trackCoverHash()));

    if (!query.exec()) {
        qCritical() << "Errore nell'inserimento della traccia:" << query.lastError().text();
        return false;
    }

    // id generato da autoincrement
    track.setId(query.lastInsertId().toInt());
    return true;
}

std::optional<Track> TrackDao::findById(int id) {
    static const auto queries = SqlParser::parseNamedQueries(":/sql/track.sql");
    const QString queryString = queries.value("findById");

    if (queryString.isEmpty()) {
        qCritical() << "Query 'findById' non trovata in track.sql";
        return std::nullopt;
    }
    qDebug() << "Preparazione esecuzione query: " << queryString;

    QSqlQuery query;
    query.prepare(queryString);
    query.bindValue(":id", id);

    if (!query.exec()) {
        qCritical() << "Errore nell'esecuzione di findById:" << query.lastError().text();
        return std::nullopt;
    }

    if (query.next()) {
        return EntityMapper::toEntityTrack(query);
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
    qDebug() << "Preparazione esecuzione query: " << queryString;

    QSqlQuery query(queryString);

    if (!query.exec()) {
        qCritical() << "Errore nell'esecuzione di getAll:" << query.lastError().text();
        return tracks;
    }

    while (query.next()) {
        tracks.append(EntityMapper::toEntityTrack(query));
    }

    return tracks;
}