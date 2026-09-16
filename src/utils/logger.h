#ifndef LOGGER_H
#define LOGGER_H

#include <QString>

class Logger
{
public:
    static void initialize(const QString& filePath);
    static void shutdown();

private:
    Logger() = delete;

    static void messageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg);
};

#endif // LOGGER_H