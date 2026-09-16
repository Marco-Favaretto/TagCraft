#include "ui/mainwindow.h"
#include "utils/logger.h"

#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    Q_INIT_RESOURCE(resources);

    const QString logDir = QDir(QCoreApplication::applicationDirPath()).filePath("logs");
    QDir().mkpath(logDir);
    const QString logPath = QDir(logDir).filePath("application.log");
    Logger::initialize(logPath);
    qInfo() << "Inizializzazione interfaccia";

    MainWindow w;
    w.show();

    const int result = QApplication::exec();

    Logger::shutdown();

    return result;
}