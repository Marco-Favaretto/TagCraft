#ifndef APPCONTROLLER_H
#define APPCONTROLLER_H

#include <QObject>
#include <QString>

#include "dto/scanresultdto.h"
#include "controller/databasecontroller.h"
#include "controller/storagecontroller.h"
#include "controller/metadatacontroller.h"
#include "controller/librarycontroller.h"

class AppController : public QObject {
    Q_OBJECT
public:
    explicit AppController(QObject* parent = nullptr);
    ~AppController() override = default;

    LibraryController* library() const;
    MetadataController* metadata() const;

    bool initialize();

public slots:
    void requestScan(const QString& path);

    void requestSaveMetadata(const QString& relativePath, const TrackDto& newValues);
    void requestSaveMetadataBatch(const QList<Track>& tracks, const QHash<QString, QVariant>& changedValues);
    void requestSetCover(const QString& relativePath, const QString& imagePath);
    void requestCleanTags(const QString& relativePath);
    void requestCleanTagsBatch(const QList<QString>& relativePaths);
    void requestRemoveCover(const QString& relativePath);
    void requestRemoveCoverBatch(const QList<QString>& relativePaths);
    void requestSetCoverBatch(const QList<QString>& relativePaths, const QString& imagePath);
    void requestSaveAlbumMetadata(const QHash<QString, QVariant>& albumChanges);
    void requestSetAlbumCover(int id, const QString& imagePath);
    void requestRemoveAlbumCover(int id);
    void requestRenameArtist(int id, const QString& newName);
    void requestRenameGenre(int id, const QString& newName);

    void requestResetDb();
    void requestResetAndRebuildDb();
    void requestScanForDevices();

    void openFS(const QString& relativePath, bool isAlbum);
    void deleteFromFS(const QString& relativePath, bool isAlbum);

    // void requestDownload(const QString& url);

signals:
    void appReady();
    void errorOccurred(const QString& message);
    void libraryUpdated();

    void storageMounted(const QString& mountPoint);
    void storageUnmounted();
    void scanProgress(int percentage);
    void scanStarted();
    void scanFinished();

    void metadataSaved(const QString& relativePath);
    void metadataSaveFailed(const QString& relativePath, const QString& reason);

    void batchOperationFinished(const QString& operationName, int succeeded, int failed);

    // void downloadProgress(const QString& url, int percentage);
    // void downloadFinished(const QString& relativePath);
    // void downloadFailed(const QString& url, const QString& reason);

private slots:
    void onScanFinished(const ScanResultDto& result);
    void onFullScanFinished(const QList<TrackFileSystemDto>& list);

private:
    QString saveMetadataFileCore(const QString& relativePath, const TrackDto& dto);
    QPair<int, int> saveMetadataBatchCore(const QList<Track>& tracks, const QHash<QString, QVariant>& changedValues, QList<TrackFileSystemDto>& updatedFilesOut);
    QString setCoverCore(const QString& relativePath, const QString& imagePath);
    QString removeCoverCore(const QString& relativePath);
    QString cleanTagsCore(const QString& relativePath);

    void setupConnections();
    void resolveArtworkFor(const QList<TrackFileSystemDto>& tracks);
    bool tryMountAndOpenDatabase();

    StorageController* m_storageController;
    MetadataController* m_metadataController;
    DatabaseController* m_databaseController;
    LibraryController* m_libraryController;
    // DownloaderController* m_downloaderController = nullptr; // futuro
};

#endif // APPCONTROLLER_H