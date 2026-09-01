#include "album.h"

Album::Album() 
    : m_id(-1), m_artistId(1) 
{}

Album::Album(int id, const QString &title, int artistId, const std::optional<QString> &coverCacheHash)
    : m_id(id), m_title(title), m_artistId(artistId), m_coverCacheHash(coverCacheHash) 
{}

int Album::id() const {
    return m_id;
}

QString Album::title() const {
    return m_title;
}

int Album::artistId() const {
    return m_artistId;
}

std::optional<int> Album::year() const {
    return m_year;
}

std::optional<QString> Album::coverCacheHash() const {
    return m_coverCacheHash;
}

QString Album::relativePath() const { 
    return m_relativePath;
}

void Album::setId(int id) {
    m_id = id;
}

void Album::setYear(std::optional<int> year) {
    m_year = year;
}

void Album::setTitle(const QString& title) {
    m_title = title;
}

void Album::setArtistId(int artistId) {
    m_artistId = artistId;
}

void Album::setCoverCacheHash(const std::optional<QString>& coverCacheHash) {
    m_coverCacheHash = coverCacheHash;
}

void Album::setRelativePath(const QString& relativePath) {
    m_relativePath = relativePath;
}

QString Album::toString() const {
    return QString("Album{id=%1, title='%2', artistId=%3}")
        .arg(m_id)
        .arg(m_title)
        .arg(m_artistId);
}