#include "detailspanel.h"

DetailsPanel::DetailsPanel(QWidget* parent)
    : QWidget(parent)
    , m_titleValue(new QLabel(this))
    , m_artistValue(new QLabel(this))
    , m_albumValue(new QLabel(this))
    , m_genreValue(new QLabel(this))
    , m_yearValue(new QLabel(this))
    , m_pathValue(new QLabel(this))
{
    auto* layout = new QFormLayout(this);
    layout->addRow("Title:", m_titleValue);
    layout->addRow("Artist ID:", m_artistValue);
    layout->addRow("Album ID:", m_albumValue);
    layout->addRow("Genre ID:", m_genreValue);
    layout->addRow("Year:", m_yearValue);
    layout->addRow("Path:", m_pathValue);

    for (QLabel* label : {m_titleValue, m_artistValue, m_albumValue,
                           m_genreValue, m_yearValue, m_pathValue}) {
        label->setWordWrap(true);
    }

    clear();
}

void DetailsPanel::showTrack(const Track& track) {
    m_titleValue->setText(track.title().isEmpty() ? "-" : track.title());
    m_artistValue->setText(QString::number(track.artistId()));
    m_albumValue->setText(QString::number(track.albumId()));
    m_genreValue->setText(track.genreId() ? QString::number(*track.genreId()) : "-");
    m_yearValue->setText(track.year() ? QString::number(*track.year()) : "-");
    m_pathValue->setText(track.relativePath());
}

void DetailsPanel::clear() {
    for (QLabel* label : {m_titleValue, m_artistValue, m_albumValue,
                           m_genreValue, m_yearValue, m_pathValue}) {
        label->setText("-");
    }
}
