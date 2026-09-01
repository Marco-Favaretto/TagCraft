#include "databasemanager.h"

#include "db/sqlparser.h"
#include "db/sqlexecutor.h"
#include "dto/constants.h"

#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QSqlError>

DatabaseManager& DatabaseManager::instance() {
    static DatabaseManager instance;
    return instance;
}

DatabaseManager::~DatabaseManager() {
    closeDatabase();
}

bool DatabaseManager::openDatabase(const QString& dbPath) {
    // Qt driver SQLite
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    
    // percorso per volume db, -> se non esiste viene creato
    // se specificato solo il nome del file, crea nella cartella di build
    // TODO da rivedere per questione di storage esterno
    m_db.setDatabaseName(dbPath);

    if (!m_db.open()) {
        qDebug() << "Errore apertura DB:" << m_db.lastError().text();
        return false;
    }

    qDebug() << "Database connesso con successo su:" << dbPath;
    return true;
}

void DatabaseManager::closeDatabase() {
    if (m_db.isOpen()) {
        m_db.close();
    }
}

bool DatabaseManager::initSchema() {
    if (!executeInitSqlScript(Constants::Sql::Schema)) {
        return false;
    }

    if (!executeInitSqlScript(Constants::Sql::StartValues)) {
        return false;
    }

    qDebug() << "Schema e dati iniziali caricati con successo";
    return true;
}

bool DatabaseManager::executeInitSqlScript(const QString& resourcePath) {
    const QStringList statements = SqlParser::parseStatements(resourcePath);

    if (statements.isEmpty()) {
        qWarning() << "Nessun comando valido trovato nello script:" << resourcePath;
        return false;
    }

    QSqlQuery query;
    for (const QString &statement : statements) {
        query.prepare(statement);
        
        if (!SqlExecutor::execute(query, {})) {
            qCritical() << "Errore durante l'esecuzione dello script:" << resourcePath;
            return false;
        }
    }

    return true;
}

bool DatabaseManager::beginTransaction() {
    return m_db.transaction();
}

bool DatabaseManager::commit() {
    return m_db.commit();
}

bool DatabaseManager::rollback() {
    return m_db.rollback();
} 

bool DatabaseManager::resetDb() {
    const QString& resourcePath = Constants::Sql::ResetDb;
    const QStringList statements = SqlParser::parseStatements(resourcePath);

    if (statements.isEmpty()) {
        qWarning() << "Nessun comando valido trovato nello script:" << resourcePath;
        return false;
    }

    QSqlQuery query;
    for (const QString& statement : statements) {
        query.prepare(statement);
        
        if (!SqlExecutor::execute(query, {})) {
            qCritical() << "Errore durante l'esecuzione dello script:" << resourcePath;
            return false;
        }
    }

    return true;
}