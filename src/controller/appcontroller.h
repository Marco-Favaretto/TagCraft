#ifndef APPCONTROLLER_H
#define APPCONTROLLER_H

#include <QObject>
#include <QString>

#include "dto/scanresultdto.h"
#include "controller/librarycontroller.h"
#include "controller/databasecontroller.h"

class AppController : public QObject {
    Q_OBJECT
public:
    explicit AppController(QObject* parent = nullptr);

private slots:
    void scanAndSync(const QString&);

private:
    LibraryController* m_libraryController;
    DatabaseController* m_databaseController;
};

#endif // APPCONTROLLER_H