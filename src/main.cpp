#include "ui/mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    Q_INIT_RESOURCE(resources);

    MainWindow w;
    w.show();

    return QApplication::exec();
}
