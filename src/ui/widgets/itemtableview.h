#ifndef ITEMTABLEVIEW_H
#define ITEMTABLEVIEW_H

#include <QWidget>
#include <QTableView>
#include <QLineEdit>
#include <QSortFilterProxyModel>
#include <QVBoxLayout>

#include "controller/librarycontroller.h"
#include "ui/models/tracktablemodel.h"
#include "ui/models/albumtablemodel.h"
#include "ui/models/artisttablemodel.h"
#include "ui/models/genretablemodel.h"
#include "dto/viewmode.h"

// per ora sola lettura
class ItemTableView : public QWidget {
    Q_OBJECT
public:
    explicit ItemTableView(LibraryController* library, QWidget* parent = nullptr);

    void setTracks(const QList<Track>& tracks);
    void setAlbums(const QList<Album>& albums);
    void setArtists(const QList<Artist>& artists);
    void setGenres(const QList<Genre>& genres);
    void sortByColumn(int column, Qt::SortOrder order);

    ViewMode currentViewMode() const;

    void clear();

signals:
    void itemSelected(int id);  // Singolo clic (aggiorna il DetailsPanel)
    void itemActivated(int id); // Doppio clic / Invio (naviga al livello successivo)

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private slots:
    void onSearchTextChanged(const QString& text);
    void onCurrentRowChanged(const QModelIndex& current, const QModelIndex& previous);
    void onDoubleClicked(const QModelIndex& index);

private:
    void activateModel(AbstractLibraryTableModel* model, ViewMode mode);
    int idForProxyIndex(const QModelIndex& proxyIndex) const;

    LibraryController* m_library; // non posseduto

    QLineEdit* m_searchBar;
    QTableView* m_tableView;
    QSortFilterProxyModel* m_proxyModel;

    TrackTableModel* m_trackModel;
    AlbumTableModel* m_albumModel;
    ArtistTableModel* m_artistModel;
    GenreTableModel* m_genreModel;

    AbstractLibraryTableModel* m_activeModel = nullptr;
    ViewMode m_currentMode = ViewMode::Tracks;
};

#endif // ITEMTABLEVIEW_H
