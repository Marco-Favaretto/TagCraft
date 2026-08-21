#ifndef TRACKDAO_H
#define TRACKDAO_H

#include "model/track.h"
#include "dto/trackfilesystemdto.h"

#include <optional>
#include <QList>

class TrackDao {
public:
    static bool insert(Track&);

    static std::optional<Track> findById(int);

    static bool update(const Track&);

    static QList<Track> getAll();

    static QHash<QString, TrackFileSystemDto> getAllFileStates(); 
};

#endif // TRACKDAO_H