#include "storage/libraryscanner.h"

#include <QFile>
#include <QDirIterator>
#include <QCoreApplication>

#include "storage/storagemanager.h"
#include "dao/trackdao.h"
#include "dto/constants.h"

QList<TrackFileSystemDto> LibraryScanner::scanAudioFiles(const QString& musicFolderPath, ProgressCallback onProgress) {
    int totalFiles = countFile(musicFolderPath);
    QList<TrackFileSystemDto> files;
    QDirIterator it(musicFolderPath, QStringList() << Constants::FileFilters::Mp3Extension, QDir::Files, QDirIterator::Subdirectories);
    int currentFile = 0;
    while (it.hasNext()) {
        it.next();
        currentFile++;
        QFileInfo info = it.fileInfo();
        files.append({
            StorageManager::instance().toRelativePath(info.absoluteFilePath()),
            info.size(),
            info.lastModified().toSecsSinceEpoch()
        });
        if (onProgress && totalFiles > 0) {
            onProgress(currentFile, totalFiles); // esegue la lamba passata come parametro
            QCoreApplication::processEvents(); // Mantiene reattiva la ProgressBar
        }
    }

    return files;
}

/*ScanResultDto LibraryScanner::smartScan(const QString& musicFolderPath) {
    QHash<QString, TrackFileSystemDto> states = TrackDao::getAllFileStates(); // HashMap<relativePath, TrackFileSystemDto>
    ScanResultDto dto;
    QSet<QString> foundPaths;
    QDirIterator it(
        musicFolderPath,
        QStringList() << Constants::FileFilters::Mp3Extension,
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
}*/

int LibraryScanner::countFile(const QString& path) {
    int totalFiles = 0;
    QDirIterator countIt(path, QStringList() << Constants::FileFilters::Mp3Extension, QDir::Files, QDirIterator::Subdirectories);
    while (countIt.hasNext()) {
        countIt.next();
        totalFiles++;
    }
    return totalFiles;
}

ScanResultDto LibraryScanner::smartScan(const QString& musicFolderPath, ProgressCallback onProgress) {
    int totalFiles = countFile(musicFolderPath);
    QHash<QString, TrackFileSystemDto> states = TrackDao::getAllFileStates();
    ScanResultDto dto;
    QSet<QString> foundPaths;
    
    QDirIterator it(musicFolderPath, QStringList() << Constants::FileFilters::Mp3Extension, QDir::Files, QDirIterator::Subdirectories);
    int currentFile = 0;

    while (it.hasNext()) {
        it.next();
        currentFile++;
        
        QFileInfo info = it.fileInfo();
        QString relPath = StorageManager::instance().toRelativePath(info.absoluteFilePath());
        foundPaths.insert(relPath);

        if (!states.contains(relPath)) {
            dto.newTracks.append({
                relPath,
                info.size(),
                info.lastModified().toSecsSinceEpoch()
            });
        } else {
            const TrackFileSystemDto& oldTrack = states[relPath];
            if (info.lastModified().toSecsSinceEpoch() != oldTrack.lastModified || info.size() != oldTrack.fileSize) {
                dto.modifiedTracks.append({
                    relPath,
                    info.size(),
                    info.lastModified().toSecsSinceEpoch()
                });
            } else {
                dto.unchangedTracks.append(relPath);
            }
        }

        if (onProgress && totalFiles > 0) {
            onProgress(currentFile, totalFiles); // esegue la lamba passata come parametro
            QCoreApplication::processEvents(); // Mantiene reattiva la ProgressBar
        }
    }

    for (auto mapIt = states.constBegin(); mapIt != states.constEnd(); ++mapIt) {
        if (!foundPaths.contains(mapIt.key())) {
            dto.deletedTracks.append(mapIt.key());
        }
    }

    return dto;
}
