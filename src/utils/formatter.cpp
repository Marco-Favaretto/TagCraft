#include "utils/formatter.h"

#include <QDateTime>

QString Formatter::formatDuration(int totalSeconds) {
    if (totalSeconds <= 0) return QString("-");
    const int minutes = totalSeconds / 60;
    const int seconds = totalSeconds % 60;
    return QString("%1:%2").arg(minutes).arg(seconds, 2, 10, QChar('0'));
}

QString Formatter::formatFileSize(qint64 bytes) {
    if (bytes <= 0) return QString("-");
    const double mb = bytes / (1024.0 * 1024.0);
    return QString("%1 MB").arg(mb, 0, 'f', 2);
}

QString Formatter::formatMtime(qint64 msSinceEpoch) {
    if (msSinceEpoch <= 0) return QString("-");
    return QDateTime::fromMSecsSinceEpoch(msSinceEpoch).toString(Qt::ISODate);
}