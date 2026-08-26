#ifndef STORAGECONTROLLER_H
#define STORAGECONTROLLER_H

#include <QObject>
#include <QList>
#include <QString>

#include "dto/scanresultdto.h"
#include "dto/trackfilesystemdto.h"

#include "storage/storagemanager.h"

class StorageController : public QObject {
    Q_OBJECT

public:
    explicit StorageController(QObject* parent = nullptr);
    ~StorageController() override = default;

    bool isStorageMounted() const;
    QString currentMountPoint() const;
    QString resolveToAbsolutePath(const QString& relativePath) const;
    QString resolveToRelativePath(const QString& absolutePath) const;
    bool validateMusicDirectory(const QString& path) const;

    ScanResultDto runScan(const QString& path);

signals:
    void storageMounted(const QString& mountPoint);
    void storageUnmounted();
    void scanProgress(int percentage);
    void scanFinished(const ScanResultDto& result);
    void errorOccurred(const QString& message);

private:
    StorageManager& storage;
    void setupConnections();
};

#endif // STORAGECONTROLLER_H