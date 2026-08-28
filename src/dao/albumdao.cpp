#include "albumdao.h"
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

bool AlbumDao::insert(Album& album) {
    static const auto queries = SqlParser::parseNamedQueries(":/sql/album.sql");
    const QString queryString = queries.value("insert");

    if (queryString.isEmpty()) {
        qCritical() << "Query 'insert' non trovata in album.sql";
        return false;
    }

    QSqlQuery query;
    query.prepare(queryString);

    if (!SqlExecutor::execute(query, {
        {":title", album.title()},
        {":artist_id", album.artistId()},
        {":cover_cache_hash", DbUtils::optionalToVariant(album.coverCacheHash())}
    })) {
        return false;
    }

    album.setId(query.lastInsertId().toInt());

    return true;
}

bool AlbumDao::update(const Album& album) {
    static const auto queries = SqlParser::parseNamedQueries(":/sql/album.sql");
    const QString queryString = queries.value("update");

    if (queryString.isEmpty()) {
        qCritical() << "Query 'update' non trovata in album.sql";
        return false;
    }

    QSqlQuery query;
    query.prepare(queryString);

    return SqlExecutor::execute(query, {
        {":id", album.id()},
        {":title", album.title()},
        {":artist_id", album.artistId()},
        {":cover_cache_hash", DbUtils::optionalToVariant(album.coverCacheHash())}
    });
}

std::optional<Album> AlbumDao::findById(int id) {
    static const auto queries = SqlParser::parseNamedQueries(":/sql/album.sql");
    const QString queryString = queries.value("findById");

    if (queryString.isEmpty()) {
        qCritical() << "Query 'findById' non trovata in album.sql";
        return std::nullopt;
    }

    QSqlQuery query;
    query.prepare(queryString);

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

    static const auto queries = SqlParser::parseNamedQueries(":/sql/album.sql");
    const QString queryString = queries.value("getAll");

    if (queryString.isEmpty()) {
        qCritical() << "Query 'getAll' non trovata in album.sql";
        return albums;
    }

    QSqlQuery query(queryString);

    if (SqlExecutor::execute(query, {})) {
        while (query.next()) {
            albums.append(EntityMapper::toEntityAlbum(query));
        }
    }

    return albums;
}

bool AlbumDao::deleteById(int id) {
    static const auto queries = SqlParser::parseNamedQueries(":/sql/album.sql");
    const QString queryString = queries.value("deleteById");
    if (queryString.isEmpty()) return false;

    QSqlQuery query;
    query.prepare(queryString);
    return SqlExecutor::execute(query, {{":id", id}});
}

std::optional<Album> AlbumDao::getByTitleAndArtist(const QString& title, int artistId) {
    static const auto queries = SqlParser::parseNamedQueries(":/sql/album.sql");
    const QString queryString = queries.value("getByTitleAndArtist");
    if (queryString.isEmpty()) return std::nullopt;

    QSqlQuery query;
    query.prepare(queryString);

    if (SqlExecutor::execute(query, {{":title", title.trimmed()}, {":artist_id", artistId}})) {
        if (query.next()) {
            return EntityMapper::toEntityAlbum(query);
        }
    }
    return std::nullopt;
}

std::optional<Album> AlbumDao::getOrCreate(const QString& name, int artistId) {
    if(auto existing = getByTitleAndArtist(name, artistId)) return *existing;

    Album album;
    album.setTitle(name);
    album.setArtistId(artistId);
    if(!insert(album)) return std::nullopt;
    else return album;
}

bool AlbumDao::deleteOrphans() {
    static const auto queries = SqlParser::parseNamedQueries(":/sql/album.sql");
    const QString queryString = queries.value("deleteOrphans");
    if (queryString.isEmpty()) return false;

    QSqlQuery query;
    query.prepare(queryString);
    return SqlExecutor::execute(query, {});
}

QList<Album> AlbumDao::getByArtistId(int artistId) {
    QList<Album> albums;
    static const auto queries = SqlParser::parseNamedQueries(":/sql/album.sql");
    const QString queryString = queries.value("getByArtistId");
    if (queryString.isEmpty()) return albums;

    QSqlQuery query;
    query.prepare(queryString);
    if (SqlExecutor::execute(query, {{":artist_id", artistId}})) {
        while (query.next()) {
            albums.append(EntityMapper::toEntityAlbum(query));
        }
    }
    return albums;
}

QList<Album> AlbumDao::searchByKeyword(const QString& keyword) {
    QList<Album> albums;
    static const auto queries = SqlParser::parseNamedQueries(":/sql/album.sql");
    const QString queryString = queries.value("searchByKeyword");
    if (queryString.isEmpty()) return albums;

    QSqlQuery query;
    query.prepare(queryString);
    QString pattern = "%" + keyword.trimmed() + "%";
    if (SqlExecutor::execute(query, {{":keyword", pattern}})) {
        while (query.next()) {
            albums.append(EntityMapper::toEntityAlbum(query));
        }
    }
    return albums;
}

bool AlbumDao::drop() {
    static const auto queries = SqlParser::parseNamedQueries(":/sql/album.sql");
    const QString queryString = queries.value("drop");
    if (queryString.isEmpty()) return false;

    QSqlQuery query;
    query.prepare(queryString);
    return SqlExecutor::execute(query, {});
}