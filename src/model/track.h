#ifndef TRACK_H
#define TRACK_H

#include <QString>
#include <QtGlobal>
#include <optional>

class Track {
public:
    Track();
    Track(int id, 
          const QString& title, 
          const QString& relativePath, 
          qint64 fileMtime, 
          qint64 fileSize,
          int artistId = 1, 
          int albumId = 1, 
          std::optional<int> genreId = 1);

    // Costruttore e assegnazione di copia, default perché non ho puntatori
    Track(const Track &other) = default;
    Track &operator=(const Track &other) = default;

    // Distruttore, default perché non ho puntatori
    ~Track() = default;

    int id() const;
    QString title() const;
    int artistId() const;
    int albumId() const;
    std::optional<int> genreId() const;
    std::optional<int> year() const;
    std::optional<int> trackNumber() const;
    std::optional<int> durationSeconds() const;
    QString relativePath() const;
    qint64 fileMtimeSecs() const;
    qint64 fileSize() const;
    std::optional<QString> trackCoverHash() const;

    void setId(int id);
    void setTitle(const QString& title);
    void setArtistId(int artistId);
    void setAlbumId(int albumId);
    void setGenreId(const std::optional<int>& genreId);
    void setYear(const std::optional<int>& year);
    void setTrackNumber(const std::optional<int>& trackNumber);
    void setDurationSeconds(const std::optional<int>& durationSeconds);
    void setRelativePath(const QString& relativePath);
    void setFileMtimeSecs(qint64 fileMtime);
    void setFileSize(qint64 fileSize);
    void setTrackCoverHash(const std::optional<QString>& trackCoverHash);

    QString toString() const;

private:
    int m_id;
    QString m_title;
    int m_artistId;
    int m_albumId;
    std::optional<int> m_genreId;
    std::optional<int> m_year;
    std::optional<int> m_trackNumber;
    std::optional<int> m_durationSeconds;

    QString m_relativePath;
    qint64 m_fileMtimeSecs;
    qint64 m_fileSize;

    std::optional<QString> m_trackCoverHash;
};

#endif // TRACK_H