#include "tracklistview.h"

TrackListView::TrackListView(QWidget* parent)
    : QWidget(parent)
    , m_listWidget(new QListWidget(this))
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_listWidget);

    connect(m_listWidget, &QListWidget::itemClicked,
            this, &TrackListView::onItemClicked);
}

void TrackListView::setTracks(const QList<Track>& tracks) {
    m_listWidget->clear();

    for (const Track& track : tracks) {
        // Titolo di fallback se mancante
        const QString label = track.title().isEmpty()
            ? track.relativePath()
            : track.title();

        auto* item = new QListWidgetItem(label, m_listWidget);
        item->setData(Qt::UserRole, track.id());
    }
}

void TrackListView::setAlbums(const QList<Album>& albums) {
    m_listWidget->clear();

    for (const Album& album : albums) {
        const QString label = album.title();

        auto* item = new QListWidgetItem(label, m_listWidget);
        item->setData(Qt::UserRole, album.id());
    }
}

void TrackListView::setArtists(const QList<Artist>& artists) {
    m_listWidget->clear();

    for (const Artist& artist : artists) {
        const QString label = artist.name();

        auto* item = new QListWidgetItem(label, m_listWidget);
        item->setData(Qt::UserRole, artist.id());
    }
}

void TrackListView::setGenres(const QList<Genre>& genres) {
    m_listWidget->clear();

    for (const Genre& genre : genres) {
        const QString label = genre.name();

        auto* item = new QListWidgetItem(label, m_listWidget);
        item->setData(Qt::UserRole, genre.id());
    }
}

void TrackListView::clear() {
    m_listWidget->clear();
}

void TrackListView::onItemClicked(QListWidgetItem* item) {
    emit itemSelected(item->data(Qt::UserRole).toInt());
}
