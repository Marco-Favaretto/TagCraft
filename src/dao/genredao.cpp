#include "genredao.h"
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

bool GenreDao::insert(Genre& genre) {
    static const auto queries = SqlParser::parseNamedQueries(":/sql/genre.sql");
    const QString queryString = queries.value("insert");

    if (queryString.isEmpty()) {
        qCritical() << "Query 'insert' non trovata in genre.sql";
        return false;
    }

    QSqlQuery query;
    if (!query.prepare(queryString)) {
        qCritical().noquote() << "[SQL PREPARE ERROR]:" << query.lastError().text();
        return false;
    }

    if (!SqlExecutor::execute(query, {
        {":name", genre.name()}
    })) {
        return false;
    }

    genre.setId(query.lastInsertId().toInt());

    return true;
}

bool GenreDao::update(const Genre& genre) {
    static const auto queries = SqlParser::parseNamedQueries(":/sql/genre.sql");
    const QString queryString = queries.value("update");

    if (queryString.isEmpty()) {
        qCritical() << "Query 'update' non trovata in genre.sql";
        return false;
    }

    QSqlQuery query;
    if (!query.prepare(queryString)) {
        qCritical().noquote() << "[SQL PREPARE ERROR]:" << query.lastError().text();
        return false;
    }

    return SqlExecutor::execute(query, {
        {":id", genre.id()},
        {":name", genre.name()}
    });
}

std::optional<Genre> GenreDao::findById(int id) {
    static const auto queries = SqlParser::parseNamedQueries(":/sql/genre.sql");
    const QString queryString = queries.value("findById");

    if (queryString.isEmpty()) {
        qCritical() << "Query 'findById' non trovata in genre.sql";
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
            return EntityMapper::toEntityGenre(query);
        }
    }

    return std::nullopt;
}

QList<Genre> GenreDao::getAll() {
    QList<Genre> genres;

    static const auto queries = SqlParser::parseNamedQueries(":/sql/genre.sql");
    const QString queryString = queries.value("getAll");

    if (queryString.isEmpty()) {
        qCritical() << "Query 'getAll' non trovata in genre.sql";
        return genres;
    }

    QSqlQuery query;
    if (!query.prepare(queryString)) {
        qCritical().noquote() << "[SQL PREPARE ERROR]:" << query.lastError().text();
        return genres;
    }

    if (SqlExecutor::execute(query, {})) {
        while (query.next()) {
            genres.append(EntityMapper::toEntityGenre(query));
        }
    }

    return genres;
}

bool GenreDao::deleteById(int id) {
    static const auto queries = SqlParser::parseNamedQueries(":/sql/genre.sql");
    const QString queryString = queries.value("deleteById");
    if (queryString.isEmpty()) return false;

    QSqlQuery query;
    if (!query.prepare(queryString)) {
        qCritical().noquote() << "[SQL PREPARE ERROR]:" << query.lastError().text();
        return false;
    }
    return SqlExecutor::execute(query, {{":id", id}});
}

std::optional<Genre> GenreDao::getByName(const QString& name) {
    static const auto queries = SqlParser::parseNamedQueries(":/sql/genre.sql");
    const QString queryString = queries.value("getByName");
    if (queryString.isEmpty()) return std::nullopt;

    QSqlQuery query;
    if (!query.prepare(queryString)) {
        qCritical().noquote() << "[SQL PREPARE ERROR]:" << query.lastError().text();
        return std::nullopt;
    }

    if (SqlExecutor::execute(query, {{":name", name.trimmed()}})) {
        if (query.next()) {
            return EntityMapper::toEntityGenre(query);
        }
    }
    return std::nullopt;
}

std::optional<Genre> GenreDao::getOrCreate(const QString& name) {
    if(auto existing = getByName(name)) return *existing;

    Genre genre;
    genre.setName(name);
    if(!insert(genre)) return std::nullopt;
    else return genre;
}

bool GenreDao::deleteOrphans() {
    static const auto queries = SqlParser::parseNamedQueries(":/sql/genre.sql");
    const QString queryString = queries.value("deleteOrphans");
    if (queryString.isEmpty()) return false;

    QSqlQuery query;
    if (!query.prepare(queryString)) {
        qCritical().noquote() << "[SQL PREPARE ERROR]:" << query.lastError().text();
        return false;
    }
    return SqlExecutor::execute(query, {});
}

QList<Genre> GenreDao::searchByKeyword(const QString& keyword) {
    QList<Genre> genres;
    static const auto queries = SqlParser::parseNamedQueries(":/sql/genre.sql");
    const QString queryString = queries.value("searchByKeyword");
    if (queryString.isEmpty()) return genres;

    QSqlQuery query;
    if (!query.prepare(queryString)) {
        qCritical().noquote() << "[SQL PREPARE ERROR]:" << query.lastError().text();
        return genres;
    }
    QString pattern = "%" + keyword.trimmed() + "%";
    if (SqlExecutor::execute(query, {{":keyword", pattern}})) {
        while (query.next()) {
            genres.append(EntityMapper::toEntityGenre(query));
        }
    }
    return genres;
}

bool GenreDao::drop() {
    static const auto queries = SqlParser::parseNamedQueries(":/sql/genre.sql");
    const QString queryString = queries.value("drop");
    if (queryString.isEmpty()) return false;

    QSqlQuery query;
    if (!query.prepare(queryString)) {
        qCritical().noquote() << "[SQL PREPARE ERROR]:" << query.lastError().text();
        return false;
    }
    return SqlExecutor::execute(query, {});
}