#ifndef FORMATTER_H
#define FORMATTER_H

#include <QString>

class Formatter {
public: 
    Formatter() = delete;

    static QString formatDuration(int totalSeconds);
    static QString formatFileSize(qint64 bytes);
    static QString formatMtime(qint64 msSinceEpoch);
};

#endif // FORMATTER_H