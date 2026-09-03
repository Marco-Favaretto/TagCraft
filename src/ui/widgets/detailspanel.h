#ifndef DETAILSPANEL_H
#define DETAILSPANEL_H

#include <QWidget>
#include <QLabel>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QPushButton>

#include "model/track.h"
#include "model/album.h"
#include "model/artist.h"
#include "model/genre.h"
#include "dto/viewmode.h"
#include "controller/librarycontroller.h"
#include "controller/metadatacontroller.h"

// per ora sola lettura poi pannello di editing metadati/artwork (§4.2/4.3)
class DetailsPanel : public QWidget {
    Q_OBJECT
public:
    explicit DetailsPanel(LibraryController* library, MetadataController* metadata, QWidget* parent = nullptr);

signals:
    void openFS(const QString& relativePath, bool isAlbum);
    void deleteFromFS(const QString& relativePath, bool isAlbum);
    void openEditModal();

public slots:
    void showTrack(const Track& track);
    void showAlbum(const Album& album);
    void showArtist(const Artist& artist);
    void showGenre(const Genre& genre);
    void clear();

private slots:
    void openFSSlot();
    void deleteFromFSSlot();

private:
    void setupUI();
    void rebuildForm(const QList<QPair<QString, QString>>& rows);
    void showArtwork(const QString& hash, ViewMode viewMode);
    void showFSButtons(const QString& relativePath, bool isAlbum);
    void hideFSButtons();

    static QString formatDuration(int totalSeconds);
    static QString formatFileSize(qint64 bytes);

    LibraryController* m_library; // non posseduto, di proprieta' di AppController
    MetadataController* m_metadata;

    QVBoxLayout* m_mainLayout;
    QFormLayout* m_formLayout;
    QLabel* m_artworkLabel;
    QPushButton* deleteButton;
    QPushButton* openFileSystemButton;
    QPushButton* openEditModalButton;

    QString m_currentRelativePath;
    bool m_currentIsAlbum = false;
    int m_lastRowCount = 0;

};

#endif // DETAILSPANEL_H
