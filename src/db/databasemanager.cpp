#include "databasemanager.h"
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
    QSqlQuery query;
    
    QString createTrackTable = R"(
        CREATE TABLE IF NOT EXISTS Track (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            title TEXT NOT NULL,
            artist_id INTEGER NOT NULL DEFAULT 1,
            album_id INTEGER NOT NULL DEFAULT 1,
            genre_id INTEGER DEFAULT 1,
            year INTEGER,
            track_number INTEGER,
            duration_seconds INTEGER,
            relative_path TEXT NOT NULL UNIQUE,
            file_mtime INTEGER NOT NULL,
            file_size INTEGER NOT NULL,
            track_cover_hash TEXT
        );
    )";

    if (!query.exec(createTrackTable)) {
        qDebug() << "Errore creazione tabella Track:" << query.lastError().text();
        return false;
    }

    return true;
}

bool DatabaseManager::executeSqlScript(const QString& resourcePath) {
    //file risorse Qt (prefisso :)
    QFile file(resourcePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Impossibile aprire il file SQL:" << resourcePath;
        return false;
    }

    QTextStream in(&file);
    QString script = in.readAll();
    file.close();

    QStringList statements = script.split(';', Qt::SkipEmptyParts);

    QSqlQuery query;
    for (QString statement : statements) {
        statement = statement.trimmed();
        
        if (statement.isEmpty() || statement.startsWith("--")) {
            continue;
        }

        if (!query.exec(statement)) {
            qDebug() << "Errore esecuzione query nello script:" << resourcePath;
            qDebug() << "Query fallita:" << statement;
            qDebug() << "Errore SQL:" << query.lastError().text();
            return false;
        }
    }

    return true;
}