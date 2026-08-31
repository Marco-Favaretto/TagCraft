#ifndef ITEMLISTVIEW_H
#define ITEMLISTVIEW_H

#include <QWidget>
#include <QListWidget>
#include <QVBoxLayout>
#include <QList>

#include "model/track.h"
#include "model/album.h"
#include "model/artist.h"
#include "model/genre.h"

class ItemListView : public QWidget {
    Q_OBJECT
public:
    explicit ItemListView(QWidget* parent = nullptr);

    void setTracks(const QList<Track>& tracks);
    void setAlbums(const QList<Album>& albums);
    void setArtists(const QList<Artist>& artists);
    void setGenres(const QList<Genre>& genres);

    void clear();

signals:
    void itemSelected(int id);

private slots:
    void onItemClicked(QListWidgetItem* item);

private:
    QListWidget* m_listWidget;
};

#endif // ITEMLISTVIEW_H