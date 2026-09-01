#include "detailspanel.h"
#include "dto/constants.h"

DetailsPanel::DetailsPanel(LibraryController* library, QWidget* parent)
    : QWidget(parent)
    , m_formLayout(new QFormLayout(this))
    , m_library(library)
{
    setMinimumWidth(250);

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

QString DetailsPanel::formatDuration(int totalSeconds) {
    if (totalSeconds <= 0) return QString();
    const int minutes = totalSeconds / 60;
    const int seconds = totalSeconds % 60;
    return QString("%1:%2").arg(minutes).arg(seconds, 2, 10, QChar('0'));
}

QString DetailsPanel::formatFileSize(qint64 bytes) {
    if (bytes <= 0) return QString();
    const double mb = bytes / (1024.0 * 1024.0);
    return QString("%1 MB").arg(mb, 0, 'f', 2);
}

void DetailsPanel::showTrack(const Track& track) {
    rebuildForm({
        {"Path:", track.relativePath()},
        {"Duration:", formatDuration(track.durationSeconds().value_or(0))},
        {"File size:", formatFileSize(track.fileSize())},
    });
}

void DetailsPanel::showAlbum(const Album& album) {
    const int trackCount = m_library->countTracksByAlbum(album.id());
    const int durationSeconds = m_library->getAlbumDurationSeconds(album.id());

    rebuildForm({
        {"Tracks:", QString::number(trackCount)},
        {"Duration:", formatDuration(durationSeconds)},
        {"Path:", album.relativePath() == Constants::DefaultValues::AlbumPath ? "-" : album.relativePath() }
    });
}

void DetailsPanel::showArtist(const Artist& artist) {
    const int albumCount = m_library->countAlbumsByArtist(artist.id());
    const int trackCount = m_library->countTracksByArtist(artist.id());

    rebuildForm({
        {"Albums:", QString::number(albumCount)},
        {"Tracks:", QString::number(trackCount)},
    });
}

void DetailsPanel::showGenre(const Genre& genre) {
    const int artistCount = m_library->countArtistsByGenre(genre.id());
    const int albumCount = m_library->countAlbumsByGenre(genre.id());
    const int trackCount = m_library->getTracksByGenre(genre.id()).size();

    rebuildForm({
        {"Artists:", QString::number(artistCount)},
        {"Albums:", QString::number(albumCount)},
        {"Tracks:", QString::number(trackCount)},
    });
}

void DetailsPanel::clear() {
    rebuildForm({});
}
