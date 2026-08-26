#include "controller/appcontroller.h"

AppController::AppController(QObject* parent) : QObject(parent) {
    m_libraryController = new LibraryController(this);
    m_databaseController = new DatabaseController(this);
}

void AppController::scanAndSync(const QString& path) {
    ScanResultDto result = m_libraryController->runScan(path);

    if(!result.newTracks.isEmpty()) m_databaseController->insertNewTracks(result.newTracks);
    if(!result.modifiedTracks.isEmpty()) m_databaseController->updateNewTracks(result.modifiedTracks);
    if(!result.deletedTracks.isEmpty()) m_databaseController->deleteNewTracks(result.deletedTracks);

}