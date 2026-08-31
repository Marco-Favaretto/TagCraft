#ifndef ITEMLISTVIEW_H
#define ITEMLISTVIEW_H

#include <QWidget>
#include <QListWidget>
#include <QVBoxLayout>
#include <QList>
#include <QKeyEvent>

#include "model/track.h"
#include "model/album.h"
#include "model/artist.h"
#include "model/genre.h"

#include "dto/viewmode.h"

class ItemListView : public QWidget {
    Q_OBJECT
public:
    explicit ItemListView(QWidget* parent = nullptr);

    void setTracks(const QList<Track>& tracks);
    void setAlbums(const QList<Album>& albums);
    void setArtists(const QList<Artist>& artists);
    void setGenres(const QList<Genre>& genres);
    
    ViewMode currentViewMode() const;

    void clear();

signals:
    void itemSelected(int id);  // Singolo clic (aggiorna il DetailsPanel)
    void itemActivated(int id); // Doppio clic / Invio (naviga al livello successivo)

protected:
    void keyPressEvent(QKeyEvent* event) override;
    
private slots:
    void onItemClicked(QListWidgetItem* item);
    void onItemDoubleClicked(QListWidgetItem* item);

private:
    QListWidget* m_listWidget;
    ViewMode m_currentMode = ViewMode::Tracks;
};

#endif // ITEMLISTVIEW_H