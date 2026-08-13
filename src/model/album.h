#ifndef ALBUM_H
#define ALBUM_H

#include <QString>
#include <optional>

class Album {
public:
    Album();
    Album(int id, const QString& title, int artistId = 1, const std::optional<QString>& coverCacheHash = std::nullopt);

    // Costruttore e assegnazione di copia, default perché non ho puntatori
    Album(const Album& other) = default;
    Album& operator=(const Album& other) = default;

    // Distruttore, default perché non ho puntatori
    ~Album() = default;

    int id() const;
    QString title() const;
    int artistId() const;
    std::optional<QString> coverCacheHash() const;

    void setId(int id);
    void setTitle(const QString& title);
    void setArtistId(int artistId);
    void setCoverCacheHash(const std::optional<QString>& coverCacheHash);

private:
    int m_id;
    QString m_title;
    int m_artistId;
    std::optional<QString> m_coverCacheHash;
};

#endif // ALBUM_H