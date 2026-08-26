#ifndef ARTISTDAO_H
#define ARTISTDAO_H

#include "model/artist.h"

#include <QString>
#include <optional>

class ArtistDao {
public:
    static bool insert(Artist& artist);
    static bool update(const Artist& artist);
    static bool deleteById(int id);

    static std::optional<Artist> findById(int id);
    static std::optional<Artist> getOrCreate(const QString&);
    static std::optional<Artist> getByName(const QString& name);
    static QList<Artist> getAll();
    static QList<Artist> searchByKeyword(const QString&);

    static bool deleteOrphans();
};

#endif // ARTISTDAO_H