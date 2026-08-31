#include "detailspanel.h"

DetailsPanel::DetailsPanel(LibraryController* library, QWidget* parent)
    : QWidget(parent)
    , m_formLayout(new QFormLayout(this))
    , m_library(library)
{
    clear();
}

void DetailsPanel::rebuildForm(const QList<QPair<QString, QString>>& rows) {
    while (m_formLayout->rowCount() > 0) {
        m_formLayout->removeRow(0);
    }
    for (const auto& row : rows) {
        auto* valueLabel = new QLabel(row.second.isEmpty() ? "-" : row.second, this);
        valueLabel->setWordWrap(true);
        m_formLayout->addRow(row.first, valueLabel);
    }
}

void DetailsPanel::showTrack(const Track& track) {
    rebuildForm({
        {"Title:", track.title()},
        {"Artist ID:", QString::number(track.artistId())},
        {"Album ID:", QString::number(track.albumId())},
        {"Genre ID:", track.genreId() ? QString::number(*track.genreId()) : QString()},
        {"Year:", track.year() ? QString::number(*track.year()) : QString()},
        {"Path:", track.relativePath()},
    });
}

void DetailsPanel::showAlbum(const Album& album) {
    auto artist = m_library->getArtistById(album.artistId());
    const int trackCount = m_library->countTracksByAlbum(album.id());

    rebuildForm({
        {"Name:", album.title()},
        {"Artist:", artist ? artist->name() : QString()},
        {"Tracks:", QString::number(trackCount)},
        {"Year:", album.year() ? QString::number(*album.year()) : QString()},
    });
}

void DetailsPanel::showArtist(const Artist& artist) {
    const int albumCount = m_library->countAlbumsByArtist(artist.id());
    const int trackCount = m_library->countTracksByArtist(artist.id());

    rebuildForm({
        {"Name:", artist.name()},
        {"Albums:", QString::number(albumCount)},
        {"Tracks:", QString::number(trackCount)},
    });
}

void DetailsPanel::showGenre(const Genre& genre) {
    const int artistCount = m_library->countArtistsByGenre(genre.id());
    const int albumCount = m_library->countAlbumsByGenre(genre.id());
    const int trackCount = m_library->getTracksByGenre(genre.id()).size();

    rebuildForm({
        {"Name:", genre.name()},
        {"Artists:", QString::number(artistCount)},
        {"Albums:", QString::number(albumCount)},
        {"Tracks:", QString::number(trackCount)},
    });
}

void DetailsPanel::clear() {
    rebuildForm({});
}