#ifndef STORAGEMANAGER_H
#define STORAGEMANAGER_H

#include <QObject>
#include <QStorageInfo>
#include <QString>

class StorageManager : public QObject {
    Q_OBJECT

public:
    explicit StorageManager(QObject* parent = nullptr);

    bool scanForStorage(); 
    bool isMounted() const;

    QString mountPoint() const; // es. "/run/media/user/externalDevice"
    
    QString toAbsolutePath(const QString&) const;
    QString toRelativePath(const QString&) const;

    QString musicPoint() const;
    QString musicAppPoint() const;

signals:
    void storageMounted(const QString& mountPoint);
    void storageUnmounted();

private:
    QStorageInfo m_activeStorage;
    bool m_isMounted{false};

    bool isValidMusicStorage(const QStorageInfo&) const;
};

#endif // STORAGEMANAGER_H