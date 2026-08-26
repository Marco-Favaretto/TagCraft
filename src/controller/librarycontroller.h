#ifndef LIBRARYCONTROLLER_H
#define LIBRARYCONTROLLER_H

#include <QObject>
#include <QString>

#include "dto/scanresultdto.h"

class LibraryController : public QObject {
    Q_OBJECT
public:
    explicit LibraryController(QObject* parent = nullptr);
    ScanResultDto runScan(const QString&);

signals:
    void errorOccurred(const QString& message);
};

#endif // LIBRARYCONTROLLER_H