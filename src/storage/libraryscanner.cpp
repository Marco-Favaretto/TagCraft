#include "storage/libraryscanner.h"

#include <QFile>
#include <QDirIterator>

#include "storage/storagemanager.h"

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