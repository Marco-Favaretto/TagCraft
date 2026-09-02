#include "albumdao.h"

#include "db/sqlparser.h"
#include "db/sqlexecutor.h"
#include "utils/dbutils.h"
#include "db/entitymapper.h"
#include "dto/constants.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QFile>
#include <QTextStream>
#include <QMap>
#include <QDebug>
#include <QFileInfo>

bool AlbumDao::insert(Album& album) {
    static const auto queries = SqlParser::parseNamedQueries(Constants::Sql::Album);
    const QString queryString = queries.value("insert");

    if (queryString.isEmpty()) {
        qCritical() << "Query 'insert' non trovata in album.sql";
        return false;
    }

    QSqlQuery query;
    if (!query.prepare(queryString)) {
        qCritical().noquote() << "[SQL PREPARE ERROR]:" << query.lastError().text();
        return false;
    }

    if (!SqlExecutor::execute(query, {
        {":title", album.title()},
        {":artist_id", album.artistId()},
        {":year", DbUtils::optionalToVariant(album.year())},
        {":relative_path", album.relativePath()},
        {":cover_cache_hash", DbUtils::optionalToVariant(album.coverCacheHash())}
    })) {
        return false;
    }

    album.setId(query.lastInsertId().toInt());

    return true;
}

bool AlbumDao::update(const Album& album) {
    static const auto queries = SqlParser::parseNamedQueries(Constants::Sql::Album);
    const QString queryString = queries.value("update");

    if (queryString.isEmpty()) {
        qCritical() << "Query 'update' non trovata in album.sql";
        return false;
    }

    QSqlQuery query;
    if (!query.prepare(queryString)) {
        qCritical().noquote() << "[SQL PREPARE ERROR]:" << query.lastError().text();
        return false;
    }

    return SqlExecutor::execute(query, {
        {":id", album.id()},
        {":title", album.title()},
        {":artist_id", album.artistId()},
        {":year", DbUtils::optionalToVariant(album.year())},
        {":relative_path", album.relativePath()},
        {":cover_cache_hash", DbUtils::optionalToVariant(album.coverCacheHash())}
    });
}

std::optional<Album> AlbumDao::findById(int id) {
    static const auto queries = SqlParser::parseNamedQueries(Constants::Sql::Album);
    const QString queryString = queries.value("findById");

    if (queryString.isEmpty()) {
        qCritical() << "Query 'findById' non trovata in album.sql";
        return std::nullopt;
    }

    QSqlQuery query;
    if (!query.prepare(queryString)) {
        qCritical().noquote() << "[SQL PREPARE ERROR]:" << query.lastError().text();
        return std::nullopt;
    }

    if (SqlExecutor::execute(query, {
        {":id", id}
    })) {
        if (query.next()) {
            return EntityMapper::toEntityAlbum(query);
        }
    }

    return std::nullopt;
}

QList<Album> AlbumDao::getAll() {
    QList<Album> albums;

    static const auto queries = SqlParser::parseNamedQueries(Constants::Sql::Album);
    const QString queryString = queries.value("getAll");

    if (queryString.isEmpty()) {
        qCritical() << "Query 'getAll' non trovata in album.sql";
        return albums;
    }

    QSqlQuery query;
    if (!query.prepare(queryString)) {
        qCritical().noquote() << "[SQL PREPARE ERROR]:" << query.lastError().text();
        return albums;
    }

    if (SqlExecutor::execute(query, {})) {
        while (query.next()) {
            albums.append(EntityMapper::toEntityAlbum(query));
        }
    }

    return albums;
}

bool AlbumDao::deleteById(int id) {
    static const auto queries = SqlParser::parseNamedQueries(Constants::Sql::Album);
    const QString queryString = queries.value("deleteById");
    if (queryString.isEmpty()) return false;

    QSqlQuery query;
    if (!query.prepare(queryString)) {
        qCritical().noquote() << "[SQL PREPARE ERROR]:" << query.lastError().text();
        return false;
    }
    return SqlExecutor::execute(query, {{":id", id}});
}

std::optional<Album> AlbumDao::getByTitleAndArtist(const QString& title, int artistId) {
    static const auto queries = SqlParser::parseNamedQueries(Constants::Sql::Album);
    const QString queryString = queries.value("getByTitleAndArtist");
    if (queryString.isEmpty()) return std::nullopt;

    QSqlQuery query;
    if (!query.prepare(queryString)) {
        qCritical().noquote() << "[SQL PREPARE ERROR]:" << query.lastError().text();
        return std::nullopt;
    }

    if (SqlExecutor::execute(query, {{":title", title.trimmed()}, {":artist_id", artistId}})) {
        if (query.next()) {
            return EntityMapper::toEntityAlbum(query);
        }
    }
    return std::nullopt;
}

std::optional<Album> AlbumDao::getByTitleAndRelativePath(const QString& title, const QString& relativePath) {
    static const auto queries = SqlParser::parseNamedQueries(Constants::Sql::Album);
    const QString queryString = queries.value("getByTitleAndRelativePath");
    if (queryString.isEmpty()) return std::nullopt;

    QSqlQuery query;
    if (!query.prepare(queryString)) {
        qCritical().noquote() << "[SQL PREPARE ERROR]:" << query.lastError().text();
        return std::nullopt;
    }

    if (SqlExecutor::execute(query, {{":title", title.trimmed()}, {":relative_path", relativePath}})) {
        if (query.next()) {
            return EntityMapper::toEntityAlbum(query);
        }
    }
    return std::nullopt;
}

std::optional<Album> AlbumDao::getOrCreate(const QString& title, const QString& trackRelativePath, int artistId, std::optional<int> year) {
    const QString albumRelativePath = QFileInfo(trackRelativePath).path();
    auto existing = getByTitleAndRelativePath(title, albumRelativePath);
    if (existing) {
        // Se anno esistente diverso da quello nuovo, album.year diventa null
        if (existing->year() != year) {
            if (existing->year().has_value()) {
                existing->setYear(std::nullopt);
                if (!update(*existing)) return std::nullopt;
            }
        }
        // se l'album esiste e ha artistId != 1 -> imposta default come artista dell'album (mantiene l'artista nella traccia però)
        if (existing->artistId() != artistId && existing->artistId() != Constants::DefaultValues::ArtistId) {
            existing->setArtistId(Constants::DefaultValues::ArtistId);
            if (!update(*existing)) return std::nullopt;
        }
        return existing;
    }

    Album album;
    album.setTitle(title);
    album.setArtistId(artistId);
    album.setYear(year);
    album.setRelativePath(albumRelativePath);
    if (!insert(album)) return std::nullopt;
    return album;
}

bool AlbumDao::deleteOrphans() {
    static const auto queries = SqlParser::parseNamedQueries(Constants::Sql::Album);
    const QString queryString = queries.value("deleteOrphans");
    if (queryString.isEmpty()) return false;

    QSqlQuery query;
    if (!query.prepare(queryString)) {
        qCritical().noquote() << "[SQL PREPARE ERROR]:" << query.lastError().text();
        return false;
    }
    return SqlExecutor::execute(query, {});
}

QList<Album> AlbumDao::getByArtistId(int artistId) {
    QList<Album> albums;
    static const auto queries = SqlParser::parseNamedQueries(Constants::Sql::Album);
    const QString queryString = queries.value("getByArtistId");
    if (queryString.isEmpty()) return albums;

    QSqlQuery query;
    if (!query.prepare(queryString)) {
        qCritical().noquote() << "[SQL PREPARE ERROR]:" << query.lastError().text();
        return albums;
    }
    if (SqlExecutor::execute(query, {{":artist_id", artistId}})) {
        while (query.next()) {
            albums.append(EntityMapper::toEntityAlbum(query));
        }
    }
    return albums;
}

QList<Album> AlbumDao::searchByKeyword(const QString& keyword) {
    QList<Album> albums;
    static const auto queries = SqlParser::parseNamedQueries(Constants::Sql::Album);
    const QString queryString = queries.value("searchByKeyword");
    if (queryString.isEmpty()) return albums;

    QSqlQuery query;
    if (!query.prepare(queryString)) {
        qCritical().noquote() << "[SQL PREPARE ERROR]:" << query.lastError().text();
        return albums;
    }
    QString pattern = "%" + keyword.trimmed() + "%";
    if (SqlExecutor::execute(query, {{":keyword", pattern}})) {
        while (query.next()) {
            albums.append(EntityMapper::toEntityAlbum(query));
        }
    }
    return albums;
}

bool AlbumDao::drop() {
    static const auto queries = SqlParser::parseNamedQueries(Constants::Sql::Album);
    const QString queryString = queries.value("drop");
    if (queryString.isEmpty()) return false;

    QSqlQuery query;
    if (!query.prepare(queryString)) {
        qCritical().noquote() << "[SQL PREPARE ERROR]:" << query.lastError().text();
        return false;
    }
    return SqlExecutor::execute(query, {});
}

bool AlbumDao::updateCoverAlbum(const QString& hash, int id) {
    static const auto queries = SqlParser::parseNamedQueries(Constants::Sql::Album);
    const QString queryString = queries.value("updateCoverAlbum");

    if (queryString.isEmpty()) {
        qCritical() << "Query 'update' non trovata in album.sql";
        return false;
    }

    QSqlQuery query;
    if (!query.prepare(queryString)) {
        qCritical().noquote() << "[SQL PREPARE ERROR]:" << query.lastError().text();
        return false;
    }

    return SqlExecutor::execute(query, {
        {":id", id},
        {":cover_cache_hash", hash}
    });
}