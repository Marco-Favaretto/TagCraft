#ifndef ALBUMDAO_H
#define ALBUMDAO_H

#include "model/album.h"

#include <QString>
#include <optional>

class AlbumDao {
public:
    static bool insert(Album &album);
    static bool update(const Album &album);

    static std::optional<Album> findById(int id);

    static QList<Album> getAll();
};

#endif // ALBUMDAO_H