#include "artistdao.h"
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


bool ArtistDao::insert(Artist& artist) {
    static const auto queries = SqlParser::parseNamedQueries(":/sql/artist.sql");
    const QString queryString = queries.value("insert");

    if (queryString.isEmpty()) {
        qCritical() << "Query 'insert' non trovata in artist.sql";
        return false;
    }

    QSqlQuery query;
    if (!query.prepare(queryString)) {
        qCritical().noquote() << "[SQL PREPARE ERROR]:" << query.lastError().text();
        return false;
    }

    if (!SqlExecutor::execute(query, {
        {":name", artist.name()}
    })) {
        return false;
    }

    artist.setId(query.lastInsertId().toInt());

    return true;
}

bool ArtistDao::update(const Artist& artist) {
    static const auto queries = SqlParser::parseNamedQueries(":/sql/artist.sql");
    const QString queryString = queries.value("update");

    if (queryString.isEmpty()) {
        qCritical() << "Query 'update' non trovata in artist.sql";
        return false;
    }

    QSqlQuery query;
    if (!query.prepare(queryString)) {
        qCritical().noquote() << "[SQL PREPARE ERROR]:" << query.lastError().text();
        return false;
    }

    return SqlExecutor::execute(query, {
        {":id", artist.id()},
        {":name", artist.name()}
    });
}

std::optional<Artist> ArtistDao::findById(int id) {
    static const auto queries = SqlParser::parseNamedQueries(":/sql/artist.sql");
    const QString queryString = queries.value("findById");

    if (queryString.isEmpty()) {
        qCritical() << "Query 'findById' non trovata in artist.sql";
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
            return EntityMapper::toEntityArtist(query);
        }
    }

    return std::nullopt;
}

QList<Artist> ArtistDao::getAll() {
    QList<Artist> artists;

    static const auto queries = SqlParser::parseNamedQueries(":/sql/artist.sql");
    const QString queryString = queries.value("getAll");

    if (queryString.isEmpty()) {
        qCritical() << "Query 'getAll' non trovata in artist.sql";
        return artists;
    }

    QSqlQuery query;
    if (!query.prepare(queryString)) {
        qCritical().noquote() << "[SQL PREPARE ERROR]:" << query.lastError().text();
        return artists;
    }

    if (SqlExecutor::execute(query, {})) {
        while (query.next()) {
            artists.append(EntityMapper::toEntityArtist(query));
        }
    }

    return artists;
}

bool ArtistDao::deleteById(int id) {
    static const auto queries = SqlParser::parseNamedQueries(":/sql/artist.sql");
    const QString queryString = queries.value("deleteById");
    if (queryString.isEmpty()) return false;

    QSqlQuery query;
    if (!query.prepare(queryString)) {
        qCritical().noquote() << "[SQL PREPARE ERROR]:" << query.lastError().text();
        return false;
    }
    return SqlExecutor::execute(query, {{":id", id}});
}

std::optional<Artist> ArtistDao::getByName(const QString& name) {
    static const auto queries = SqlParser::parseNamedQueries(":/sql/artist.sql");
    const QString queryString = queries.value("getByName");
    if (queryString.isEmpty()) return std::nullopt;

    QSqlQuery query;
    if (!query.prepare(queryString)) {
        qCritical().noquote() << "[SQL PREPARE ERROR]:" << query.lastError().text();
        return std::nullopt;
    }

    if (SqlExecutor::execute(query, {{":name", name.trimmed()}})) {
        if (query.next()) {
            return EntityMapper::toEntityArtist(query);
        }
    }
    return std::nullopt;
}

std::optional<Artist> ArtistDao::getOrCreate(const QString& name) {
    if(auto existing = getByName(name)) return *existing;

    Artist artist;
    artist.setName(name);
    if(!insert(artist)) return std::nullopt;
    else return artist;
}

bool ArtistDao::deleteOrphans() {
    static const auto queries = SqlParser::parseNamedQueries(":/sql/artist.sql");
    const QString queryString = queries.value("deleteOrphans");
    if (queryString.isEmpty()) return false;

    QSqlQuery query;
    if (!query.prepare(queryString)) {
        qCritical().noquote() << "[SQL PREPARE ERROR]:" << query.lastError().text();
        return false;
    }
    return SqlExecutor::execute(query, {});
}

QList<Artist> ArtistDao::searchByKeyword(const QString& keyword) {
    QList<Artist> artists;
    static const auto queries = SqlParser::parseNamedQueries(":/sql/artist.sql");
    const QString queryString = queries.value("searchByKeyword");
    if (queryString.isEmpty()) return artists;

    QSqlQuery query;
    if (!query.prepare(queryString)) {
        qCritical().noquote() << "[SQL PREPARE ERROR]:" << query.lastError().text();
        return artists;
    }
    
    QString pattern = "%" + keyword.trimmed() + "%";
    if (SqlExecutor::execute(query, {{":keyword", pattern}})) {
        while (query.next()) {
            artists.append(EntityMapper::toEntityArtist(query));
        }
    }
    return artists;
}

bool ArtistDao::drop() {
    static const auto queries = SqlParser::parseNamedQueries(":/sql/artist.sql");
    const QString queryString = queries.value("drop");
    if (queryString.isEmpty()) return false;

    QSqlQuery query;
    if (!query.prepare(queryString)) {
        qCritical().noquote() << "[SQL PREPARE ERROR]:" << query.lastError().text();
        return false;
    }
    return SqlExecutor::execute(query, {});
}