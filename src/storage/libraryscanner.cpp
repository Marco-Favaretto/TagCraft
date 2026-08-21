#include "storage/libraryscanner.h"

#include <QFile>
#include <QDirIterator>

#include "storage/storagemanager.h"
#include "dao/trackdao.h"

QList<TrackFileSystemDto> LibraryScanner::scanAudioFiles(const QString& musicFolderPath) {
    QList<TrackFileSystemDto> files;
    QDirIterator it(musicFolderPath, QStringList() << "*.mp3", QDir::Files, QDirIterator::Subdirectories);
    
    while (it.hasNext()) {
        it.next();
        QFileInfo info = it.fileInfo();
        files.append({
            StorageManager::instance().toRelativePath(info.absoluteFilePath()),
            info.size(),
            info.lastModified().toSecsSinceEpoch()
        });
    }

    return files;
}

ScanResultDto LibraryScanner::smartScan(const QString& musicFolderPath) {
    QHash<QString, TrackFileSystemDto> states = TrackDao::getAllFileStates();
    ScanResultDto dto;
    QDirIterator it(musicFolderPath, QStringList() << "*.mp3", QDir::Files, QDirIterator::Subdirectories);

    while(it.hasNext()) {
        it.next();
        QFileInfo info = it.fileInfo();

        QString relPath = StorageManager::instance().toRelativePath(info.absoluteFilePath());

        // if relPath not in Map -> newSong
        // if relpath in map but lastmodified diff -> changed
        // if relpath in map but not in iterator -> deleted
        // if relpath in map and lastmodified equals -> unchanged

    }

    return dto;
}