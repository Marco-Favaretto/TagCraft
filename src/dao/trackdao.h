#ifndef TRACKDAO_H
#define TRACKDAO_H

#include "model/track.h"
#include <optional>
#include <QList>

class TrackDao {
public:
    static bool insert(Track &track);

    static std::optional<Track> findById(int id);

    static QList<Track> getAll();
};

#endif // TRACKDAO_H