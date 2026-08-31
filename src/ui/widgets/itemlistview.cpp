#include "itemlistview.h"

ItemListView::ItemListView(QWidget* parent)
    : QWidget(parent)
    , m_listWidget(new QListWidget(this))
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_listWidget);

    connect(m_listWidget, &QListWidget::itemClicked, this, &ItemListView::onItemClicked);
    connect(m_listWidget, &QListWidget::itemDoubleClicked, this, &ItemListView::onItemDoubleClicked);
}

void ItemListView::setTracks(const QList<Track>& tracks) {
    m_listWidget->clear();
    m_currentMode = ViewMode::Tracks;

    for (const Track& track : tracks) {
        // Titolo di fallback se mancante
        const QString label = track.title().isEmpty()
            ? track.relativePath()
            : track.title();

        auto* item = new QListWidgetItem(label, m_listWidget);
        item->setData(Qt::UserRole, track.id());
    }
}

void ItemListView::setAlbums(const QList<Album>& albums) {
    m_listWidget->clear();
    m_currentMode = ViewMode::Albums;

    for (const Album& album : albums) {
        const QString label = album.title();

        auto* item = new QListWidgetItem(label, m_listWidget);
        item->setData(Qt::UserRole, album.id());
    }
}

void ItemListView::setArtists(const QList<Artist>& artists) {
    m_listWidget->clear();
    m_currentMode = ViewMode::Artists;

    for (const Artist& artist : artists) {
        const QString label = artist.name();

        auto* item = new QListWidgetItem(label, m_listWidget);
        item->setData(Qt::UserRole, artist.id());
    }
}

void ItemListView::setGenres(const QList<Genre>& genres) {
    m_listWidget->clear();
    m_currentMode = ViewMode::Genres;

    for (const Genre& genre : genres) {
        const QString label = genre.name();

        auto* item = new QListWidgetItem(label, m_listWidget);
        item->setData(Qt::UserRole, genre.id());
    }
}

void ItemListView::clear() {
    m_listWidget->clear();
}

void ItemListView::onItemClicked(QListWidgetItem* item) {
    emit itemSelected(item->data(Qt::UserRole).toInt());
}

void ItemListView::onItemDoubleClicked(QListWidgetItem* item) {
    if (!item) return;
    int id = item->data(Qt::UserRole).toInt();
    emit itemActivated(id);
}

void ItemListView::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        QListWidgetItem* currentItem = m_listWidget->currentItem();
        if (currentItem) {
            int id = currentItem->data(Qt::UserRole).toInt();
            emit itemActivated(id);
            return;
        }
    }
    QWidget::keyPressEvent(event);
}

ViewMode ItemListView::currentViewMode() const { return m_currentMode; }