#ifndef DETAILSPANEL_H
#define DETAILSPANEL_H

#include <QWidget>
#include <QLabel>
#include <QFormLayout>
#include <QVBoxLayout>

#include "model/track.h"
#include "model/album.h"
#include "model/artist.h"
#include "model/genre.h"
#include "controller/librarycontroller.h"
#include "controller/metadatacontroller.h"

// per ora sola lettura poi pannello di editing metadati/artwork (§4.2/4.3)
class DetailsPanel : public QWidget {
    Q_OBJECT
public:
    explicit DetailsPanel(LibraryController* library, MetadataController* metadata, QWidget* parent = nullptr);

public slots:
    void showTrack(const Track& track);
    void showAlbum(const Album& album);
    void showArtist(const Artist& artist);
    void showGenre(const Genre& genre);
    void clear();

private:
    void rebuildForm(const QList<QPair<QString, QString>>& rows);
    void showArtwork(const QString& hash, bool isAlbum);
    static QString formatDuration(int totalSeconds);
    static QString formatFileSize(qint64 bytes);

    QVBoxLayout* m_mainLayout;
    QFormLayout* m_formLayout;
    QLabel* m_artworkLabel;
    int m_lastRowCount = 0;
    LibraryController* m_library; // non posseduto, di proprieta' di AppController
    MetadataController* m_metadata;
};

#endif // DETAILSPANEL_H
