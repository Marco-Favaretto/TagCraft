#include "track.h"

Track::Track()
    : m_id(-1), m_artistId(1), m_albumId(1), m_genreId(1), m_fileMtime(0), m_fileSize(0) 
{}

Track::Track(int id, 
             const QString& title, 
             const QString& relativePath, 
             qint64 fileMtime, 
             qint64 fileSize,
             int artistId, 
             int albumId, 
             std::optional<int> genreId)
    : m_id(id), 
      m_title(title), 
      m_artistId(artistId), 
      m_albumId(albumId), 
      m_genreId(genreId),
      m_relativePath(relativePath), 
      m_fileMtime(fileMtime), 
      m_fileSize(fileSize) 
{}

int Track::id() const {
    return m_id;
}

QString Track::title() const {
    return m_title;
}

int Track::artistId() const {
    return m_artistId;
}

int Track::albumId() const {
    return m_albumId;
}

std::optional<int> Track::genreId() const {
    return m_genreId;
}

std::optional<int> Track::year() const {
    return m_year;
}

std::optional<int> Track::trackNumber() const {
    return m_trackNumber;
}

std::optional<int> Track::durationSeconds() const {
    return m_durationSeconds;
}

QString Track::relativePath() const {
    return m_relativePath;
}

qint64 Track::fileMtime() const {
    return m_fileMtime;
}

qint64 Track::fileSize() const {
    return m_fileSize;
}

std::optional<QString> Track::trackCoverHash() const {
    return m_trackCoverHash;
}

void Track::setId(int id) {
    m_id = id;
}

void Track::setTitle(const QString& title) {
    m_title = title;
}

void Track::setArtistId(int artistId) {
    m_artistId = artistId;
}

void Track::setAlbumId(int albumId) {
    m_albumId = albumId;
}

void Track::setGenreId(const std::optional<int>& genreId) {
    m_genreId = genreId;
}

void Track::setYear(const std::optional<int>& year) {
    m_year = year;
}

void Track::setTrackNumber(const std::optional<int>& trackNumber) {
    m_trackNumber = trackNumber;
}

void Track::setDurationSeconds(const std::optional<int>& durationSeconds) {
    m_durationSeconds = durationSeconds;
}

void Track::setRelativePath(const QString& relativePath) {
    m_relativePath = relativePath;
}

void Track::setFileMtime(qint64 fileMtime) {
    m_fileMtime = fileMtime;
}

void Track::setFileSize(qint64 fileSize) {
    m_fileSize = fileSize;
}

void Track::setTrackCoverHash(const std::optional<QString>& trackCoverHash) {
    m_trackCoverHash = trackCoverHash;
}

QString Track::toString() const {
    return QString("Track(%1, %2, artist=%3, album=%4, path=%5)")
        .arg(m_id)
        .arg(m_title)
        .arg(m_artistId)
        .arg(m_albumId)
        .arg(m_relativePath);
}