#ifndef GENREDAO_H
#define GENREDAO_H

#include "model/genre.h"

#include <QString>
#include <optional>

class GenreDao {
public:
    static bool insert(Genre& genre);
    static bool update(const Genre& genre);
    static bool deleteById(int id);

    static std::optional<Genre> findById(int id);
    static std::optional<Genre> getByName(const QString& name);
    static QList<Genre> getAll();
};

#endif // GENREDAO_H