#include "ui/mainwindow.h"

#include "db/databasemanager.h"
#include "dao/trackdao.h"
#include "dao/artistdao.h"
#include "dao/albumdao.h"
#include "model/track.h"
#include "model/artist.h"
#include "model/album.h"

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
    
    auto fetchedTrack = TrackDao::getAll();
    if (!fetchedTrack.isEmpty()) {
        for(Track t : fetchedTrack) qDebug() << "Traccia:" << t.toString();
    } else {
        return false;
    }
    
    auto fetchedAlbum = AlbumDao::getAll();
    if (!fetchedAlbum.isEmpty()) {
        for(Album t : fetchedAlbum) qDebug() << "album:" << t.toString();
    } else {
        return false;
    }
    
    auto fetchedArtist = ArtistDao::getAll();
    if (!fetchedArtist.isEmpty()) {
        for(Artist t : fetchedArtist) qDebug() << "artista:" << t.toString();
    } else {
        return false;
    }

    return true;
}