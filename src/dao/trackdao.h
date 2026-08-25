#ifndef TRACKDAO_H
#define TRACKDAO_H

#include "model/track.h"
#include "dto/trackfilesystemdto.h"

#include <optional>
#include <QList>

class TrackDao {
public:
    static bool insert(Track& track);
    static bool update(const Track& track);
    static bool deleteById(int id);

    static std::optional<Track> findById(int id);
    static QList<Track> getAll();
    static QHash<QString, TrackFileSystemDto> getAllFileStates(); 
};

#endif // TRACKDAO_H