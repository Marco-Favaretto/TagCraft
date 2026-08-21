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
    QHash<QString, TrackFileSystemDto> states = TrackDao::getAllFileStates(); // HashMap<relativePath, TrackFileSystemDto>
    ScanResultDto dto;
    QSet<QString> foundPaths;
    QDirIterator it(
        musicFolderPath,
        QStringList() << "*.mp3",
        QDir::Files,
        QDirIterator::Subdirectories
    );

    while (it.hasNext()) {
        it.next();
        QFileInfo info = it.fileInfo();
        QString relPath = StorageManager::instance().toRelativePath(info.absoluteFilePath());
        foundPaths.insert(relPath);

        if (!states.contains(relPath)) { // NEW: if relPath not in Map -> newSong
            dto.newTracks.append({
                relPath,
                info.size(),
                info.lastModified().toSecsSinceEpoch()
            });
            qDebug() << "Track" << relPath << "nuova";
        } else {
            const TrackFileSystemDto& oldTrack = states[relPath];
            if (info.lastModified().toSecsSinceEpoch() != oldTrack.lastModified || info.size() != oldTrack.fileSize) { // MODIFIED: if relpath in map but lastmodified diff -> changed
                dto.modifiedTracks.append({
                    relPath,
                    info.size(),
                    info.lastModified().toSecsSinceEpoch()
                });
                qDebug() << "Track" << relPath << "modificata";
            } else { // UNCHANGED:  if relpath in map and lastmodified equals -> unchanged
                dto.unchangedTracks.append(relPath);
                qDebug() << "Track" << relPath << "non modificata";
            }
        }
    }

    // DELETED: if relpath in map but not in DB/iterator -> deleted
    for (auto it = states.constBegin(); it != states.constEnd(); ++it) {
        if (!foundPaths.contains(it.key())) {
            dto.deletedTracks.append(it.key());

            qDebug() << "Track" << it.key() << "eliminata";
        }
    }

    return dto;
}
