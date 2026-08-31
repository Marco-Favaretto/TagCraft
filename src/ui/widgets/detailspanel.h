#ifndef DETAILSPANEL_H
#define DETAILSPANEL_H

#include <QWidget>
#include <QLabel>
#include <QFormLayout>

#include "model/track.h"
#include "model/album.h"
#include "model/artist.h"
#include "model/genre.h"
#include "controller/librarycontroller.h"

class DetailsPanel : public QWidget {
    Q_OBJECT
public:
    explicit DetailsPanel(LibraryController* library, QWidget* parent = nullptr);

public slots:
    void showTrack(const Track& track);
    void showAlbum(const Album& album);
    void showArtist(const Artist& artist);
    void showGenre(const Genre& genre);
    void clear();

protected:
    void resizeEvent(QResizeEvent* event) override;
private:
    void rebuildForm(const QList<QPair<QString, QString>>& rows);

    QFormLayout* m_formLayout;
    int m_lastRowCount = 0;
    LibraryController* m_library;
};

#endif // DETAILSPANEL_H
