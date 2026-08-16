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
#include "storage/storagemanager.h"


void testDb();
void testStorage();

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    // MainWindow w;
    // w.show();
    Q_INIT_RESOURCE(resources);
    
    // testDb();
    testStorage();


    // return QApplication::exec();
    return 0;
}

void testStorage() {
    qDebug() << "avvio test storage";

    StorageManager storagemanager;

    if(storagemanager.scanForStorage()) qDebug() << "storage trovato";
    else qDebug() << "scan fallita";

    if(storagemanager.isMounted()) {
        qDebug() << "Mount point: " << storagemanager.mountPoint();
        QString pathDaDb = "artisti/Pink Floyd/1973 The Dark Side of the Moon/02 - Breathe (In The Air) - The Dark Side Of The Moon - Pink Floyd.mp3";
        QString absolutePathBreathe = storagemanager.toAbsolutePath(pathDaDb);
        qDebug() << "absolute Path di Breathe (In The Air): " << absolutePathBreathe;
        qDebug() << "path relativo di Breathe (In The Air): " << storagemanager.toRelativePath(absolutePathBreathe);
    }


    qDebug() << "fine test storage";
}

void testDb() {
    qDebug() << "testDb Avvio";

    // connessione db -> attualmente su cartella progetto, in futuro path storage esterno
    if (!DatabaseManager::instance().openDatabase("music_library.db")) {
        qDebug() << "testDb fallito";
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
    //     qDebug() << "testDb fallito";
    // }
    
    auto fetchedTrack = TrackDao::getAll();
    if (!fetchedTrack.isEmpty()) {
        for(Track t : fetchedTrack) qDebug() << "Traccia:" << t.toString();
    } else {
        qDebug() << "testDb fallito";
    }
    
    auto fetchedAlbum = AlbumDao::getAll();
    if (!fetchedAlbum.isEmpty()) {
        for(Album t : fetchedAlbum) qDebug() << "album:" << t.toString();
    } else {
        qDebug() << "testDb fallito";
    }
    
    auto fetchedArtist = ArtistDao::getAll();
    if (!fetchedArtist.isEmpty()) {
        for(Artist t : fetchedArtist) qDebug() << "artista:" << t.toString();
    } else {
        qDebug() << "testDb fallito";
    }

    DatabaseManager::instance().closeDatabase();

    qDebug() << "testDb fine";
}