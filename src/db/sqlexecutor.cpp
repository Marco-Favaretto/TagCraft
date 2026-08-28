#include "sqlexecutor.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariantMap>
#include <QDebug>
#include <QRegularExpression>
#include <utility>

QString SqlExecutor::sanitizeQuery(QString queryStr) {    
    return queryStr.replace(QRegularExpression("[\\r\\n\\t]+"), " ")
                    .replace(QRegularExpression("\\s+"), " ")
                    .trimmed();
}

QString SqlExecutor::formatValue(const QVariant& val) {
    if (val.isNull()) return "NULL";
    
    QString str = val.toString();
    
    if (val.typeId() == QMetaType::QString) {
        return QString("\"%1\"").arg(str);
    }
    return str;
}

void SqlExecutor::logQuery(const QString& rawQuery, const QVariantMap& params) {
    QString cleanQuery = sanitizeQuery(rawQuery);

    if (params.isEmpty()) {
        qDebug().noquote() << "EXEC SQL:" << cleanQuery;
        return;
    }

    QStringList paramList;
    for (auto it = params.constBegin(); it != params.constEnd(); ++it) {
        paramList.append(QString("%1: %2").arg(it.key(), formatValue(it.value())));
    }

    qDebug().noquote() << QString("EXEC SQL: %1 | PARAMS: { %2 }")
                            .arg(cleanQuery, paramList.join(", "));
}

bool SqlExecutor::execute(QSqlQuery& query, const QVariantMap& params = {}) {
    // Bind parametri con nome (:id, :nome, ecc.)
    for (auto it = params.constBegin(); it != params.constEnd(); ++it) {
        query.bindValue(it.key(), it.value());
    }

    logQuery(query.lastQuery(), params);

    // Esecuzione query
    if (!query.exec()) {
        qCritical().noquote() << "[SQL ERROR]:" << query.lastError().text();
        qCritical().noquote() << "[FAILED QUERY]:" << sanitizeQuery(query.executedQuery());
        return false;
    }

    return true;
}

// Passa elenco coppie { ":chiave", valore }
bool SqlExecutor::execute(QSqlQuery& query, std::initializer_list<std::pair<QString, QVariant>> params) {
    QVariantMap map;
    for (const auto& pair : params) {
        map.insert(pair.first, pair.second);
    }
    return execute(query, map);
}

