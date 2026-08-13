#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QString>
#include <QSqlDatabase>

class DatabaseManager {
public:
    static DatabaseManager& instance();

    bool openDatabase(const QString &dbPath);
    void closeDatabase();

    bool initSchema();

private:
    DatabaseManager() = default;
    ~DatabaseManager();

    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    bool executeSqlScript(const QString&);

    QSqlDatabase m_db;
};

#endif // DATABASEMANAGER_H