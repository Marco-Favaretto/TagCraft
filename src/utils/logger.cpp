#include "logger.h"

#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <QDebug>

namespace {
    QFile logFile;
}

void Logger::initialize(const QString& filePath) {
    logFile.setFileName(filePath);

    if (!logFile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
        qWarning() << "Impossibile aprire il file di log:" << logFile.errorString();
        return;
    }

    qInstallMessageHandler(Logger::messageHandler);

    qInfo() << "========================================";
    qInfo() << "Applicazione avviata";
    qInfo() << "========================================";
}

void Logger::shutdown() {
    if (logFile.isOpen()) {
        qInfo() << "Applicazione terminata";
        logFile.close();
    }

    qInstallMessageHandler(nullptr);
}

void Logger::messageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
    Q_UNUSED(context);
    QString level;

    switch (type) {
        case QtDebugMsg:
            level = "DEBUG";
            break;
        case QtInfoMsg:
            level = "INFO";
            break;
        case QtWarningMsg:
            level = "WARNING";
            break;
        case QtCriticalMsg:
            level = "CRITICAL";
            break;
        case QtFatalMsg:
            level = "FATAL";
            break;
    }

    const QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss.zzz");
    const QString line = QString("[%1] [%2] %3").arg(timestamp, level, msg);

    QTextStream stream(&logFile);
    stream << line << '\n';
    stream.flush();

    fprintf(stderr, "%s\n", line.toLocal8Bit().constData());

    if (type == QtFatalMsg) abort();
}
