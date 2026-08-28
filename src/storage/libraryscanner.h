#ifndef LIBRARYSCANNER_H
#define LIBRARYSCANNER_H

#include "dto/trackdto.h"
#include "dto/trackfilesystemdto.h"
#include "dto/scanresultdto.h"

#include <QList>
#include <QString>

#include <functional>

class LibraryScanner {
public:
    LibraryScanner() = delete;
    
    // Callback tipo: std::function<void(int attuali, int totali)>
    using ProgressCallback = std::function<void(int, int)>;

    static QList<TrackFileSystemDto> scanAudioFiles(const QString& musicFolderPath, ProgressCallback onProgress = nullptr);

    static ScanResultDto smartScan(const QString& musicFolderPath, ProgressCallback onProgress = nullptr);

private:
    static int countFile(const QString&);
};

#endif // LIBRARYSCANNER_H