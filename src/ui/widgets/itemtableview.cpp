#include "itemtableview.h"

#include <QHeaderView>
#include <QKeyEvent>

ItemTableView::ItemTableView(LibraryController* library, QWidget* parent)
    : QWidget(parent)
    , m_library(library)
    , m_searchBar(new QLineEdit(this))
    , m_tableView(new QTableView(this))
    , m_proxyModel(new QSortFilterProxyModel(this))
    , m_trackModel(new TrackTableModel(this))
    , m_albumModel(new AlbumTableModel(this))
    , m_artistModel(new ArtistTableModel(this))
    , m_genreModel(new GenreTableModel(this))
{
    m_searchBar->setPlaceholderText("Search...");

    m_proxyModel->setSortRole(Qt::EditRole);
    m_proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    // Filtra su tutte le colonne, non solo la prima
    m_proxyModel->setFilterKeyColumn(-1);

    m_tableView->setModel(m_proxyModel);
    m_tableView->setSortingEnabled(true);
    m_tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_tableView->setEditTriggers(QAbstractItemView::NoEditTriggers); // sola lettura

    m_tableView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // m_tableView->horizontalHeader()->setStretchLastSection(true);
    m_tableView->verticalHeader()->setVisible(false);
    m_tableView->installEventFilter(this);

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(4);
    layout->addWidget(m_searchBar);
    layout->addWidget(m_tableView);

    connect(m_searchBar, &QLineEdit::textChanged, this, &ItemTableView::onSearchTextChanged);
    connect(m_tableView, &QTableView::doubleClicked, this, &ItemTableView::onDoubleClicked);

    activateModel(m_trackModel, ViewMode::Tracks);
}

void ItemTableView::activateModel(AbstractLibraryTableModel* model, ViewMode mode) {
    m_activeModel = model;
    m_currentMode = mode;
    m_proxyModel->setSourceModel(model);

    connect(m_tableView->selectionModel(), &QItemSelectionModel::currentRowChanged,
            this, &ItemTableView::onCurrentRowChanged, Qt::UniqueConnection);

    m_tableView->resizeColumnsToContents();
    m_tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);  
    m_tableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
}

void ItemTableView::setTracks(const QList<Track>& tracks) {
    QList<TrackTableModel::Row> rows;
    rows.reserve(tracks.size());

    for (const Track& t : tracks) {
        auto artist = m_library->getArtistById(t.artistId());
        auto album = m_library->getAlbumById(t.albumId());

        TrackTableModel::Row row;
        row.id = t.id();
        row.title = t.title();
        row.artistName = artist ? artist->name() : QString();
        row.albumName = album ? album->title() : QString();
        row.trackNumber = t.trackNumber().value_or(0);
        row.year = t.year().value_or(0);
        rows.append(row);
    }

    m_trackModel->setRows(rows);
    m_tableView->sortByColumn(0, Qt::AscendingOrder);
    activateModel(m_trackModel, ViewMode::Tracks);
}

void ItemTableView::setAlbums(const QList<Album>& albums) {
    QList<AlbumTableModel::Row> rows;
    rows.reserve(albums.size());

    for (const Album& a : albums) {
        auto artist = m_library->getArtistById(a.artistId());

        AlbumTableModel::Row row;
        row.id = a.id();
        row.title = a.title();
        row.artistName = artist ? artist->name() : QString();
        row.year = a.year().value_or(0);
        rows.append(row);
    }

    m_albumModel->setRows(rows);
    m_tableView->sortByColumn(0, Qt::AscendingOrder);
    activateModel(m_albumModel, ViewMode::Albums);
}

void ItemTableView::setArtists(const QList<Artist>& artists) {
    QList<ArtistTableModel::Row> rows;
    rows.reserve(artists.size());

    for (const Artist& a : artists) {
        ArtistTableModel::Row row;
        row.id = a.id();
        row.name = a.name();
        rows.append(row);
    }

    m_artistModel->setRows(rows);
    m_tableView->sortByColumn(0, Qt::AscendingOrder);
    activateModel(m_artistModel, ViewMode::Artists);
}

void ItemTableView::setGenres(const QList<Genre>& genres) {
    QList<GenreTableModel::Row> rows;
    rows.reserve(genres.size());

    for (const Genre& g : genres) {
        GenreTableModel::Row row;
        row.id = g.id();
        row.name = g.name();
        rows.append(row);
    }

    m_genreModel->setRows(rows);
    m_tableView->sortByColumn(0, Qt::AscendingOrder);
    activateModel(m_genreModel, ViewMode::Genres);
}

void ItemTableView::clear() {
    m_activeModel = nullptr;
    m_trackModel->setRows({});
    m_albumModel->setRows({});
    m_artistModel->setRows({});
    m_genreModel->setRows({});
}

ViewMode ItemTableView::currentViewMode() const {
    return m_currentMode;
}

void ItemTableView::onSearchTextChanged(const QString& text) {
    m_proxyModel->setFilterFixedString(text);
}

int ItemTableView::idForProxyIndex(const QModelIndex& proxyIndex) const {
    if (!proxyIndex.isValid() || !m_activeModel) return -1;
    const QModelIndex sourceIndex = m_proxyModel->mapToSource(proxyIndex);
    return m_activeModel->idAt(sourceIndex.row());
}

void ItemTableView::onCurrentRowChanged(const QModelIndex& current, const QModelIndex& previous) {
    Q_UNUSED(previous);
    const int id = idForProxyIndex(current);
    if (id >= 0) emit itemSelected(id);
}

void ItemTableView::onDoubleClicked(const QModelIndex& index) {
    const int id = idForProxyIndex(index);
    if (id >= 0) emit itemActivated(id);
}

bool ItemTableView::eventFilter(QObject* watched, QEvent* event) {
    if (watched == m_tableView && event->type() == QEvent::KeyPress) {
        auto* keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) {
            const int id = idForProxyIndex(m_tableView->currentIndex());
            if (id >= 0) {
                emit itemActivated(id);
                return true;
            }
        }
    }
    return QWidget::eventFilter(watched, event);
}

void ItemTableView::sortByColumn(int column, Qt::SortOrder order) {
    m_tableView->sortByColumn(column, order);
}