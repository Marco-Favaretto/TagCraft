#ifndef APPCONTROLLER_H
#define APPCONTROLLER_H

#include <QObject>
#include <QString>

#include "dto/scanresultdto.h"
#include "controller/databasecontroller.h"
#include "controller/storagecontroller.h"

class AppController : public QObject {
    Q_OBJECT
public:
    explicit AppController(QObject* parent = nullptr);

private slots:
    void scanAndSync(const QString&);

signals:
    void errorOccurred(const QString&);
    void libraryUpdated();

private:
    DatabaseController* m_databaseController;
    StorageController* m_storageController;
};

#endif // APPCONTROLLER_H