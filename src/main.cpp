#include "ui/mainwindow.h"

#include "db/databasemanager.h"
#include "dao/trackdao.h"
#include "dao/artistdao.h"
#include "dao/albumdao.h"
#include "model/track.h"
#include "model/artist.h"
#include "model/album.h"
#include "dao/genredao.h"
#include "model/genre.h"
#include "storage/storagemanager.h"
#include "dto/trackdto.h"
#include "utils/tagmapper.h"

#include <iostream>

#include <QApplication>
#include <QDebug>
#include <QImage>

#include <mpegfile.h>
#include <id3v2tag.h>
#include <attachedpictureframe.h>
#include <fileref.h>
#include <tpropertymap.h>

void testDb(const StorageManager&);
void testStorage(StorageManager&);
void testTagLib(const StorageManager&);

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    // MainWindow w;
    // w.show();
    Q_INIT_RESOURCE(resources);
    
    StorageManager storage;
    testStorage(storage);
    qDebug() << "\n";
    testDb(storage);
    qDebug() << "\n";
    testTagLib(storage);
    qDebug() << "\n";

    // return QApplication::exec();
    return 0;
}

void testStorage(StorageManager& storageManager) {
    qDebug() << "avvio test storage";

    if(storageManager.scanForStorage()) qDebug() << "storage trovato";
    else qDebug() << "scan fallita";

    if(storageManager.isMounted()) {
        qDebug() << "Mount point: " << storageManager.mountPoint();
        QString pathDaDb = "artisti/Pink Floyd/1973 The Dark Side of the Moon/02 - Breathe (In The Air) - The Dark Side Of The Moon - Pink Floyd.mp3";
        QString absolutePathBreathe = storageManager.toAbsolutePath(pathDaDb);
        qDebug() << "absolute Path di Breathe (In The Air): " << absolutePathBreathe;
        qDebug() << "path relativo di Breathe (In The Air): " << storageManager.toRelativePath(absolutePathBreathe);
    }

    qDebug() << "fine test storage";
}

void testDb(const StorageManager& storageManager) {
    qDebug() << "testDb Avvio";

    // connessione db -> attualmente su cartella progetto, in futuro path storage esterno
    if (!DatabaseManager::instance().openDatabase(storageManager.musicAppPoint() + "/" + "music_library.db")) {
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
        for(const Track& t : fetchedTrack) qDebug() << "Traccia:" << t.toString();
    } else {
        qDebug() << "nessuna tracca";
    }
    
    auto fetchedAlbum = AlbumDao::getAll();
    if (!fetchedAlbum.isEmpty()) {
        for(const Album& t : fetchedAlbum) qDebug() << "album:" << t.toString();
    } else {
        qDebug() << "nessun album";
    }
    
    auto fetchedArtist = ArtistDao::getAll();
    if (!fetchedArtist.isEmpty()) {
        for(const Artist& t : fetchedArtist) qDebug() << "artista:" << t.toString();
    } else {
        qDebug() << "nessun artista";
    }

    auto fetchedGenre = GenreDao::getAll();
    if (!fetchedGenre.isEmpty()) {
        for(const Genre& t : fetchedGenre) qDebug() << "genre:" << t.toString();
    } else {
        qDebug() << "nessun genere";
    }

    DatabaseManager::instance().closeDatabase();

    qDebug() << "testDb fine";
}

void testTagLib(const StorageManager& storageManager) {
    qDebug() << "testTagLib inizio";

    QString path = storageManager.toAbsolutePath("Music/ost/metal gear/30 - MGSV - The Man Who Sold The World.mp3");
    
    TrackDto t = TagMapper::fileToDto(path, storageManager.toRelativePath(path));    
    QByteArray byteArray = TagMapper::extractEmbeddedCover(path);

    QImage image;
    if (image.loadFromData(byteArray)) {
        image.save("../tmp/cover.jpg");
    }

    qDebug() << "testTagLib fine";
}
