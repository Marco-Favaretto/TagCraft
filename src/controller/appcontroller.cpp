#include "controller/appcontroller.h"

AppController::AppController(QObject* parent) : QObject(parent) {
    m_databaseController = new DatabaseController(this);
    m_storageController = new StorageController(this);
}

void AppController::scanAndSync(const QString& path) {
    ScanResultDto result = m_storageController->runScan(path);

    if(!result.newTracks.isEmpty()) m_databaseController->insertNewTracks(result.newTracks);
    if(!result.modifiedTracks.isEmpty()) m_databaseController->updateNewTracks(result.modifiedTracks);
    if(!result.deletedTracks.isEmpty()) m_databaseController->deleteNewTracks(result.deletedTracks);
}