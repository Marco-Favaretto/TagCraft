#include "storage/storagemanager.h"

#include <QDir>

StorageManager& StorageManager::instance() {
    static StorageManager instance;
    return instance;
}

StorageManager::StorageManager(QObject* parent) : 
    QObject(parent), 
    m_activeStorage(), 
    m_isMounted(false) {}

bool StorageManager::scanForStorage() {
    QList<QStorageInfo> deviceList = QStorageInfo::mountedVolumes();
    m_isMounted = false;
    for(auto device : deviceList) {
        if(isValidMusicStorage(device)) {
            m_activeStorage = device;
            m_isMounted = true;
            emit storageMounted(m_activeStorage.rootPath());
            break;
        }
    }
    if (!m_isMounted) emit storageUnmounted();
    return m_isMounted;
}

bool StorageManager::isMounted() const {
    return m_isMounted && m_activeStorage.isReady();
}

QString StorageManager::mountPoint() const {
    if(isMounted()) return m_activeStorage.rootPath();
    else return "";
}

QString StorageManager::musicPoint() const {
    QDir rootDir(this->mountPoint());
    QString musicPoint = "";
    const QFileInfoList entries = rootDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    for (const QFileInfo& entry : entries) {
        if (entry.fileName().compare("Music", Qt::CaseInsensitive) == 0)
            musicPoint = entry.absoluteFilePath();
    }

    return musicPoint;
}

QString StorageManager::musicAppPoint() const {
    return this->mountPoint() + "/.music_app";
}

QString StorageManager::toAbsolutePath(const QString& relativePath) const {
    if (!isMounted()) return QString();
    return QDir(m_activeStorage.rootPath()).absoluteFilePath(relativePath);
}

QString StorageManager::toRelativePath(const QString& absolutePath) const {
    if (!isMounted()) return QString();
    return QDir(m_activeStorage.rootPath()).relativeFilePath(absolutePath);
}

bool StorageManager::isValidMusicStorage(const QStorageInfo& device) const {
    // bool okMusic = false;
    // bool okApp = false;
    // QDir dir(device.rootPath());
    // const QFileInfoList entries = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
    // for (const QFileInfo& entry : entries) {
    //     const QString name = entry.fileName().toLower();
    //     if (name == "music") okMusic = true;
    //     else if (name == ".music_app") okApp = true;
    //     if (okMusic && okApp) return true;
    // }
    // return false;

    QDir rootDir(device.rootPath());
    return rootDir.exists("Music") && rootDir.exists(".music_app");
}


QString StorageManager::artworkCacheDirectory() const {
    return this->mountPoint() + "./music_app/covers";
}