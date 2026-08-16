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
    TagLib::FileRef f(path.toStdString().c_str());

    if (f.isNull() || !f.file()) {
        qDebug() << "Impossibile aprire il file:" << path;
        return;
    }

    TagLib::MPEG::File* mpegFile = dynamic_cast<TagLib::MPEG::File*>(f.file());

    if (!mpegFile) {
        qDebug() << "Il file non è un MP3";
        return;
    }

    TagLib::ID3v2::Tag *tag = mpegFile->ID3v2Tag(); // Accesso diretto al tag ID3v2

    if (!tag) {
        qDebug() << "Il file non contiene un tag ID3v2";
        return;
    }

    qDebug() << "ARTIST:" << tag->artist().toCString();
    qDebug() << "TITLE:" << tag->title().toCString();
    qDebug() << "ALBUM:" << tag->album().toCString();
    qDebug() << "GENRE:" << tag->genre().toCString();
    qDebug() << "YEAR:" << tag->year();
    qDebug() << "TRACK:" << tag->track();

    // APIC = Attached Picture
    const auto frames = tag->frameList("APIC");

    qDebug() << "Numero APIC:" << frames.size();

    for (const auto *frame : frames) {

        const TagLib::ID3v2::AttachedPictureFrame* picture = dynamic_cast<const TagLib::ID3v2::AttachedPictureFrame*>(frame);

        if (!picture) continue;

        qDebug() << "MIME:" << picture->mimeType().toCString();
        qDebug() << "Descrizione:" << picture->description().toCString();
        qDebug() << "Tipo:" << picture->type();

        const TagLib::ByteVector data = picture->picture();

        QImage image;
        if (!image.loadFromData(
                reinterpret_cast<const uchar *>(data.data()),
                static_cast<int>(data.size()))) {

            qDebug() << "Impossibile decodificare l'immagine";
            continue;
        }

        qDebug() << "Immagine:" << image.width() << "x" << image.height();
        image.save("../tmp/cover.jpg");
    }

    qDebug() << "testTagLib fine";
}
