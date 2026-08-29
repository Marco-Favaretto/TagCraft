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

void TrackListView::clear() {
    m_listWidget->clear();
}

void TrackListView::onItemClicked(QListWidgetItem* item) {
    emit trackSelected(item->data(Qt::UserRole).toInt());
}
