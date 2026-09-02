#include "detailspanel.h"

#include "dto/constants.h"

DetailsPanel::DetailsPanel(LibraryController* library, MetadataController* metadata, QWidget* parent)
    : QWidget(parent)
    , m_mainLayout(new QVBoxLayout(this))
    , m_formLayout(new QFormLayout(this))
    , m_artworkLabel(new QLabel(this))
    , m_library(library)
    , m_metadata(metadata)
{
    setMinimumWidth(250);
    
    m_artworkLabel->setAlignment(Qt::AlignCenter);
    m_artworkLabel->setScaledContents(false);

    m_mainLayout->setContentsMargins(10, 10, 10, 10);
    m_mainLayout->setSpacing(15);
    m_mainLayout->addWidget(m_artworkLabel, 0, Qt::AlignCenter);

    m_formLayout->setContentsMargins(10, 10, 10, 10);
    m_formLayout->setSpacing(8);
    m_formLayout->setLabelAlignment(Qt::AlignRight | Qt::AlignTop);
    m_formLayout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
    m_formLayout->setRowWrapPolicy(QFormLayout::DontWrapRows);

    m_mainLayout->addLayout(m_formLayout);
    m_mainLayout->addStretch();

    clear();
}

void DetailsPanel::rebuildForm(const QList<QPair<QString, QString>>& rows) {
    while (m_formLayout->rowCount() > 0) m_formLayout->removeRow(0);

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
    showArtwork(track.trackCoverHash() ? *(track.trackCoverHash()) : "", false);
    rebuildForm({
        {"Path:", track.relativePath()},
        {"Duration:", formatDuration(track.durationSeconds().value_or(0))},
        {"File size:", formatFileSize(track.fileSize())},
    });
}

void DetailsPanel::showAlbum(const Album& album) {
    showArtwork(album.coverCacheHash() ? *(album.coverCacheHash()) : "", true);
    const int trackCount = m_library->countTracksByAlbum(album.id());
    const int durationSeconds = m_library->getAlbumDurationSeconds(album.id());

    rebuildForm({
        {"Tracks:", QString::number(trackCount)},
        {"Duration:", formatDuration(durationSeconds)},
        {"Path:", album.relativePath() == Constants::DefaultValues::AlbumPath ? "-" : album.relativePath() }
    });
}

void DetailsPanel::showArtist(const Artist& artist) {
    m_artworkLabel->clear();
    m_artworkLabel->setVisible(false);

    const int albumCount = m_library->countAlbumsByArtist(artist.id());
    const int trackCount = m_library->countTracksByArtist(artist.id());

    rebuildForm({
        {"Albums:", QString::number(albumCount)},
        {"Tracks:", QString::number(trackCount)},
    });
}

void DetailsPanel::showGenre(const Genre& genre) {
    m_artworkLabel->clear();
    m_artworkLabel->setVisible(false);

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
    m_artworkLabel->clear();
    m_artworkLabel->setVisible(false);
    rebuildForm({});
}

void DetailsPanel::showArtwork(const QString& hash, bool isAlbum) {
    QImage image;
    std::optional<QImage> imagePtr = m_metadata->loadFromCache(hash);
    if(!imagePtr) {
        if(isAlbum) image.load(Constants::Artwork::AlbumArtwork);
        else image.load(Constants::Artwork::TrackArtwork);
    } else image = *imagePtr;

    if (!image.isNull()) {
        QPixmap pixmap = QPixmap::fromImage(image).scaled(
            QSize(200, 200), 
            Qt::KeepAspectRatio, 
            Qt::SmoothTransformation
        );
        m_artworkLabel->setPixmap(pixmap);
        m_artworkLabel->setVisible(true);
    } else {
        m_artworkLabel->clear();
        m_artworkLabel->setVisible(false);
    }
}
