#include "sqlexecutor.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QVariantMap>
#include <QDebug>
#include <utility>

bool SqlExecutor::execute(QSqlQuery& query, const QVariantMap& params = {}) {
    // Bind parametri con nome (:id, :nome, ecc.)
    for (auto it = params.constBegin(); it != params.constEnd(); ++it) {
        query.bindValue(it.key(), it.value());
    }

    // Esecuzione query
    if (!query.exec()) {
        qCritical().noquote() << "[SqlExecutor Error]:" << query.lastError().text();
        qCritical().noquote() << "[Executed Query]:" << query.executedQuery();
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

