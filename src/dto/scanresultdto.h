#ifndef SCANRESULTDTO_H
#define SCANRESULTDTO_H

#include <QList>
#include "trackfilesystemdto.h"

// da usare per smart scan
struct ScanResultDto {
    QList<TrackFileSystemDto> newTracks;
    QList<TrackFileSystemDto> modifiedTracks;
    QList<QString> deletedTracks;    // relative_path dei record DB non trovati sul filesystem
    QList<QString> unchangedTracks;  // relative_path, solo per log/debug/conteggio
};

#endif // SCANRESULTDTO_H