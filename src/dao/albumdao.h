#ifndef ALBUMDAO_H
#define ALBUMDAO_H

#include "model/album.h"

#include <QString>
#include <optional>

class AlbumDao {
public:
    static bool insert(Album& album);
    static bool update(const Album& album);
    static bool deleteById(int id);
    static bool drop();

    static std::optional<Album> findById(int id);
    static std::optional<Album> getByTitleAndArtist(const QString& title, int artistId);
    static std::optional<Album> getOrCreate(const QString&, int);
    static QList<Album> getAll();
    static QList<Album> getByArtistId(int);
    static QList<Album> searchByKeyword(const QString&);
    
    static bool deleteOrphans();
};

#endif // ALBUMDAO_H