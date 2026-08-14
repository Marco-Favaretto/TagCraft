#include "ui/mainwindow.h"

#include "db/databasemanager.h"
#include "dao/trackdao.h"
#include "model/track.h"

#include <QApplication>
#include <QDebug>


bool testDb();

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    // MainWindow w;
    // w.show();
    Q_INIT_RESOURCE(resources);
    qDebug() << "testDb Avvio";
    if(testDb()) {
        qDebug() << "testDb fine";
    } else {
        qDebug() << "testDb fallito";
    }
    DatabaseManager::instance().closeDatabase();
    // return QApplication::exec();
    return 0;
}


bool testDb() {
    // connessione db -> attualmente su cartella progetto, in futuro path storage esterno
    if (!DatabaseManager::instance().openDatabase("music_library.db")) {
        return false;
    }

    DatabaseManager::instance().initSchema();

    // Track newTrack; // id iniziale è -1
    // newTrack.setTitle("Pigs (Three Different Ones)");
    // newTrack.setRelativePath("artisti/Pink Floyd/1977 Animals/02 - Pigs.mp3");
    // newTrack.setFileMtime(1600000000);
    // newTrack.setFileSize(15400300);
    // newTrack.setYear(1977);

    // if (TrackDao::insert(newTrack)) {
    //     qDebug() << "Traccia salvata con successo. Nuovo ID generato da SQLite:" << newTrack.id();
    // } else {
    //     return false;
    // }
    
    auto fetchedTrack = TrackDao::findById(1);
    if (fetchedTrack.has_value()) {
        qDebug() << "Traccia letta dal DB:" << fetchedTrack->toString();
    } else {
        return false;
    }

    return true;
}