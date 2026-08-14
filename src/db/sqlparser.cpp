#include "sqlparser.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include<QRegularExpression>

QStringList SqlParser::parseStatements(const QString &resourcePath) {
    QStringList statements;
    QFile file(resourcePath);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "SqlParser: Impossibile aprire il file:" << resourcePath;
        return statements;
    }

    QTextStream in(&file);
    QString script = in.readAll();
    file.close();

    static const QRegularExpression commentRegex("--[^\n]*");
    script.remove(commentRegex);

    const QStringList rawStatements = script.split(';', Qt::SkipEmptyParts);

    for (QString statement : rawStatements) {
        statement = statement.trimmed();

        // Salta blocchi vuoti o linee che iniziano con commenti
        if (statement.isEmpty() || statement.startsWith("--")) {
            continue;
        }

        statements.append(statement);
    }

    return statements;
}

QMap<QString, QString> SqlParser::parseNamedQueries(const QString &resourcePath) {
    QMap<QString, QString> queries;
    QFile file(resourcePath);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "SqlParser: Impossibile aprire il file:" << resourcePath;
        return queries;
    }

    QTextStream in(&file);
    QString currentQueryName;
    QString currentQueryText;

    while (!in.atEnd()) {
        QString line = in.readLine();
        
        // Cerca etichetta '-- name: nomeQuery'
        if (line.startsWith("-- name:")) {
            if (!currentQueryName.isEmpty()) {
                QString cleanQuery = currentQueryText.trimmed();
                // Rimuove l'ultimo ';' se presente a fine query per evitare errori in QSqlQuery
                if (cleanQuery.endsWith(';')) {
                    cleanQuery.chop(1);
                }
                queries[currentQueryName] = cleanQuery.trimmed();
            }
            currentQueryName = line.section("-- name:", 1, 1).trimmed();
            currentQueryText.clear();
        } else {
            currentQueryText += line + "\n";
        }
    }

    // Aggiunge ultima query
    if (!currentQueryName.isEmpty()) {
        QString cleanQuery = currentQueryText.trimmed();
        if (cleanQuery.endsWith(';')) {
            cleanQuery.chop(1);
        }
        queries[currentQueryName] = cleanQuery.trimmed();
    }

    file.close();
    return queries;
}