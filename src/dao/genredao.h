#ifndef GENREDAO_H
#define GENREDAO_H

#include "model/genre.h"

#include <QString>
#include <optional>

class GenreDao {
public:
    static bool insert(Genre&);
    static bool update(const Genre&);

    static std::optional<Genre> findById(int);

    static QList<Genre> getAll();
};

#endif // GENREDAO_H