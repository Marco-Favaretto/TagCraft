#include "detailspanel.h"

#include <QResizeEvent>

DetailsPanel::DetailsPanel(LibraryController* library, QWidget* parent)
    : QWidget(parent)
    , m_library(library)
{
    setMinimumWidth(250);

    m_formLayout = new QFormLayout(this);
    m_formLayout->setContentsMargins(10, 10, 10, 10);
    m_formLayout->setSpacing(8);
    m_formLayout->setLabelAlignment(Qt::AlignRight | Qt::AlignTop);
    m_formLayout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
    m_formLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);

    clear();
}

void DetailsPanel::rebuildForm(const QList<QPair<QString, QString>>& rows) {
    while (m_formLayout->rowCount() > 0) {
        m_formLayout->removeRow(0);
    }

    for (const auto& row : rows) {
        auto* valueLabel = new QLabel(row.second.isEmpty() ? "-" : row.second, this);
        valueLabel->setWordWrap(true);
        valueLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
        m_formLayout->addRow(row.first, valueLabel);
    }
}

void DetailsPanel::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    m_formLayout->invalidate();
    m_formLayout->activate();
}

void DetailsPanel::showTrack(const Track& track) {
    QString title = track.title();
    QString artist;
    auto artOpt = m_library->getArtistById(track.artistId());
    if(artOpt) artist = artOpt->name() + " (" + QString::number(track.artistId()) + ")";
    QString album;
    auto alOpt = m_library->getAlbumById(track.albumId());
    if(alOpt) album = alOpt->title() + " (" + QString::number(track.albumId()) + ")";
    QString genre;
    if(track.genreId()) {
        auto genreOpt = m_library->getGenreById(*track.genreId());
        if(genreOpt) {
            genre = genreOpt->name() + " (" + QString::number(*track.genreId()) + ")";
        }
    }
    rebuildForm({
        {"Title:", title},
        {"Artist:", artist},
        {"Album ID:", album},
        {"Genre ID:", genre},
        {"Year:", track.year() ? QString::number(*track.year()) : QString()},
        {"Path:", track.relativePath()},
    });
}

void DetailsPanel::showAlbum(const Album& album) {
    QString title = album.title();

    QString artist;
    auto artistOpt = m_library->getArtistById(album.artistId());
    if(artistOpt) {
        artist = artistOpt->name() + " (" + QString::number(album.artistId()) + ")";
    }

    const int trackCount = m_library->countTracksByAlbum(album.id());

    rebuildForm({
        {"Title:", title},
        {"Artist:", artist},
        {"Tracks:", QString::number(trackCount)},
        {"Year:", album.year() ? QString::number(*album.year()) : QString()},
    });
}

void DetailsPanel::showArtist(const Artist& artist) {
    QString name = artist.name();

    const int albumCount = m_library->countAlbumsByArtist(artist.id());
    const int trackCount = m_library->countTracksByArtist(artist.id());

    rebuildForm({
        {"Name:", name},
        {"Albums:", QString::number(albumCount)},
        {"Tracks:", QString::number(trackCount)},
    });
}

void DetailsPanel::showGenre(const Genre& genre) {
    QString name = genre.name();

    const int artistCount = m_library->countArtistsByGenre(genre.id());
    const int albumCount = m_library->countAlbumsByGenre(genre.id());
    const int trackCount = m_library->getTracksByGenre(genre.id()).size();

    rebuildForm({
        {"Name:", name},
        {"Artists:", QString::number(artistCount)},
        {"Albums:", QString::number(albumCount)},
        {"Tracks:", QString::number(trackCount)},
    });
}

void DetailsPanel::clear() {
    rebuildForm({});
}