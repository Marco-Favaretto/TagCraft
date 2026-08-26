#include "controller/librarycontroller.h"

#include "storage/libraryscanner.h"

LibraryController::LibraryController(QObject* parent) : QObject(parent) {}

ScanResultDto LibraryController::runScan(const QString& path) {
    return LibraryScanner::smartScan(path);
    // emit errorOccured("Errore durante la scan");
}