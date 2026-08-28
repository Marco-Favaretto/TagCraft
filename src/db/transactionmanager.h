#ifndef TRANSACTIONMANAGER_H
#define TRANSACTIONMANAGER_H

#include "db/databasemanager.h"

class TransactionManager {
    public:
    explicit TransactionManager(DatabaseManager&);
    
    // Disabilita la copia per sicurezza
    TransactionManager(const TransactionManager&) = delete;
    TransactionManager& operator=(const TransactionManager&) = delete;
    
    ~TransactionManager();
    
    bool isStarted() const;
    bool commit();
    
private:
    DatabaseManager& m_db;
    bool m_started{false};
    bool m_committed{false};
};

#endif // TRANSACTIONMANAGER_H