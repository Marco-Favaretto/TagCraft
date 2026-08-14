#ifndef SQLEXECUTOR_H
#define SQLEXECUTOR_H

#include <QSqlQuery>
#include <QSqlError>
#include <QVariantMap>
#include <QDebug>
#include <utility>

class SqlExecutor {
private:
    
    static bool execute(QSqlQuery&, const QVariantMap&);
    
    static QString sanitizeQuery(QString);
    static QString formatValue(const QVariant&);
    static void logQuery(const QString&, const QVariantMap&);
        
public:
    SqlExecutor() = delete; // Impedisce l'istanziamento

    // Passa elenco coppie { ":chiave", valore }
    static bool execute(QSqlQuery&, std::initializer_list<std::pair<QString, QVariant>>);
};

#endif // SQLEXECUTOR_H