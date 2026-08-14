#ifndef ARTISTDAO_H
#define ARTISTDAO_H

#include "model/artist.h"

#include <QString>
#include <optional>

class ArtistDao {
public:
    static bool insert(Artist&);
    static bool update(const Artist&);

    static std::optional<Artist> findById(int);

    static QList<Artist> getAll();
};

#endif // ARTISTDAO_H