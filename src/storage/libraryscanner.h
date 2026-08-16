#ifndef LIBRARYSCANNER_H
#define LIBRARYSCANNER_H

#include "dto/trackdto.h"
#include "dto/trackfilesystemdto.h"

#include <QList>
#include <QString>

class LibraryScanner {
public:
    LibraryScanner() = delete;
    static QList<TrackFileSystemDto> scanAudioFiles(const QString&);
};

#endif // LIBRARYSCANNER_H