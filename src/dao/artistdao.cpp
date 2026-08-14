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
    query.prepare(queryString);

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
    query.prepare(queryString);

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
    query.prepare(queryString);

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

    QSqlQuery query(queryString);

    if (SqlExecutor::execute(query, {})) {
        while (query.next()) {
            artists.append(EntityMapper::toEntityArtist(query));
        }
    }

    return artists;
}