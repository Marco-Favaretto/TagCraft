#include "db/transactionmanager.h"

TransactionManager::TransactionManager(DatabaseManager& db) : m_db(db), m_committed(false) {
    m_started = m_db.beginTransaction();
}

TransactionManager::~TransactionManager() {
    // Se transazione è iniziata ma non confermata, rollback automatico
    if (m_started && !m_committed) {
        m_db.rollback();
    }
}

bool TransactionManager::isStarted() const {
    return m_started;
}

bool TransactionManager::commit() {
    if (m_started && !m_committed) {
        m_committed = m_db.commit();
        return m_committed;
    }
    return false;
}

