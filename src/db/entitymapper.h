#ifndef ENTITYMAPPER_H
#define ENTITYMAPPER_H

#include "model/track.h"
#include <QSqlQuery>

class EntityMapper {
public:
    EntityMapper() = delete;

    static Track toEntityTrack(const QSqlQuery& query);
};

#endif // ENTITYMAPPER_H