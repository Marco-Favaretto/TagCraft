#ifndef LIBRARYSCANNER_H
#define LIBRARYSCANNER_H

#include "dto/trackdto.h"
#include "dto/trackfilesystemdto.h"
#include "dto/scanresultdto.h"

#include <QList>
#include <QString>

class LibraryScanner {
public:
    LibraryScanner() = delete;
    static QList<TrackFileSystemDto> scanAudioFiles(const QString&);
    static ScanResultDto smartScan();
};

#endif // LIBRARYSCANNER_H