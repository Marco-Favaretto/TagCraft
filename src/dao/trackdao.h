#ifndef TRACKDAO_H
#define TRACKDAO_H

#include "model/track.h"
#include <optional>
#include <QList>

class TrackDao {
public:
    static bool insert(Track&);

    static std::optional<Track> findById(int);
    static bool update(const Track&);

    static QList<Track> getAll();
};

#endif // TRACKDAO_H