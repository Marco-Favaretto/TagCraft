#ifndef SQLPARSER_H
#define SQLPARSER_H

#include <QString>
#include <QStringList>
#include <QMap>

class SqlParser {
public:
    SqlParser() = delete; // solo static

    // usato per lo script init, divide gli statement
    static QStringList parseStatements(const QString& resourcePath);

    // [nomeQuery] -> [testoQuery]
    static QMap<QString, QString> parseNamedQueries(const QString& resourcePath);
};

#endif // SQLPARSER_H