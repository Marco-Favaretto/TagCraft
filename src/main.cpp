#include "ui/mainwindow.h"

#include "db/databasemanager.h"
#include "dao/trackdao.h"
#include "dao/artistdao.h"
#include "dao/albumdao.h"
#include "dao/genredao.h"
#include "model/track.h"
#include "model/artist.h"
#include "model/album.h"
#include "model/genre.h"
#include "storage/storagemanager.h"
#include "storage/libraryscanner.h"
#include "dto/trackdto.h"
#include "utils/tagmapper.h"

#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QApplication>
#include <QDebug>
#include <QImage>

#include <mpegfile.h>
#include <id3v2tag.h>
#include <attachedpictureframe.h>
#include <fileref.h>
#include <tpropertymap.h>

std::unordered_map<std::string, std::string> properties;

void loadProperties() {
    std::ifstream file("../tmp/application.properties");
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
        auto pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            properties[key] = value;
        }
    }
}

void testDb();
void testStorage();
void testTagFromFile();
void testTagToFile();
void testCleanTags();
void saveToMp3(const QString&, const QString&);
void testFullScan();
void testSmartScan();
QJsonObject trackToJson(const TrackFileSystemDto&);

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    // if (!StorageManager::instance().scanForStorage()) {
    // gestione storage non trovato 
    // (copia locale db, e sync due versioni una volta riconnesso o copia locale e visualizzazione senza modifiche)
    // }
    // DatabaseManager::instance().openDatabase(/* path calcolato da StorageManager */);
    // DatabaseManager::instance().initSchema();

    // MainWindow w;
    // w.show();

    // int ret = QApplication::exec();
    // DatabaseManager::instance().closeDatabase();
    // return ret;

    Q_INIT_RESOURCE(resources);
    loadProperties();

    QString command;
    QTextStream stream(stdin);

    while (true) {
        qDebug() << "Comandi disponibili: [dbstorage] [readfile] [writetags] [cleantags] [fullscan] [smartscan] [exit]";
        std::cout << "~ ";
        stream >> command;

        if (command == "dbstorage") {
            testDb();
        } 
        else if (command == "readfile") {
            testTagFromFile();
        } 
        else if (command == "writetags") {
            testTagToFile();
        } 
        else if (command == "cleantags") {
            testCleanTags();
        } 
        else if (command == "fullscan") {
            testFullScan();
        } 
        else if (command == "smartscan") {
            testSmartScan();
        } 
        else if (command == "exit") {
            break;
        }
    }
    return 0;
}

void testStorage() {
    qDebug() << "avvio test storage";
    StorageManager& storageManager = StorageManager::instance();

    if(storageManager.scanForStorage()) qDebug() << "storage trovato";
    else qDebug() << "scan fallita";

    if(storageManager.isMounted()) {
        qDebug() << "Mount point: " << storageManager.mountPoint();
        QString pathDaDb = QString::fromStdString(properties["testStorage"]);
        QString absolutePathBreathe = storageManager.toAbsolutePath(pathDaDb);
        qDebug() << "absolute Path: " << absolutePathBreathe;
        qDebug() << "path relativo: " << storageManager.toRelativePath(absolutePathBreathe);
    }

    qDebug() << "fine test storage";
}

void testDb() {
    qDebug() << "testDb Avvio";

    if(!StorageManager::instance().isMounted()) testStorage();
    const StorageManager& storageManager = StorageManager::instance();
    
    // connessione db -> attualmente su cartella progetto, in futuro path storage esterno
    if (!DatabaseManager::instance().openDatabase(storageManager.musicAppPoint() + "/" + "music_library.db")) {
        qDebug() << "testDb fallito";
    }
    
    DatabaseManager::instance().initSchema();
    
    qDebug() << "inserimento traccia: " << QString::fromStdString(properties["testdb.title"]) << "con artista, album e genere di default.";
    // Track newTrack; // id iniziale è -1
    // newTrack.setTitle(QString::fromStdString(properties["testdb.title"]));
    // newTrack.setRelativePath(QString::fromStdString(properties["testdb.relPath"]));
    // newTrack.setFileMtime(std::stoi(properties["testdb.filemtime"]));
    // newTrack.setFileSize(std::stoi(properties["testdb.filesize"]));
    // newTrack.setYear(std::stoi(properties["testdb.year"]));
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

void testTagFromFile() {
    qDebug() << "testTagFromFile inizio";

    if(!StorageManager::instance().isMounted()) testStorage();
    const StorageManager& storageManager = StorageManager::instance();

    QString path = storageManager.toAbsolutePath(QString::fromStdString(properties["testTagFromFile"]));
    
    TrackDto t = TagMapper::fileToDto(path, storageManager.toRelativePath(path));    
    QByteArray byteArray = TagMapper::extractEmbeddedCover(path);

    QImage image;
    if (image.loadFromData(byteArray)) image.save("../tmp/extractcover/cover.jpg");

    qDebug() << "testTagFromFile fine";
}

void testTagToFile() {
    qDebug() << "testTagToFile inizio";
    QString path = QString::fromStdString(properties["testTagToFile.path"]);
    QString pathImage = QString::fromStdString(properties["testTagToFile.pathImage"]);
    QString targetDir = QString::fromStdString(properties["testTagToFile.target"]);
    
    TrackDto t(
        QString::fromStdString(properties["testTagToFile.relPath"]),
        QString::fromStdString(properties["testTagToFile.title"]),
        QString::fromStdString(properties["testTagToFile.artist"]),
        QString::fromStdString(properties["testTagToFile.album"]),
        QString::fromStdString(properties["testTagToFile.genre"]),
        std::stoi(properties["testTagToFile.tracknumber"]),
        std::stoi(properties["testTagToFile.year"])
    );
    
    bool tags = TagMapper::dtoToFile(path, t);
    bool cover = TagMapper::embedCover(path, pathImage);
    if(tags && cover) {
        qDebug() << "tag aggiornati";
        saveToMp3(targetDir, path);
    }
    qDebug() << "testTagToFile fine";
}

void testCleanTags() {
    qDebug() << "testCLeanTags inizio";
    QString path = QString::fromStdString(properties["testTagToFile.path"]);
    QString targetDir = QString::fromStdString(properties["testTagToFile.target"]);
    bool clean = TagMapper::cleanTags(path);
    if(clean) {
        qDebug() << "tag aggiornati";
        saveToMp3(targetDir, path);
    }
    qDebug() << "testCLeanTags fine";
}

void saveToMp3(const QString& targetDir, const QString& origin) {
    QDir dir(targetDir);

    if (!dir.mkpath(".")) {
        qWarning() << "Impossibile creare la directory:" << targetDir;
        return;
    }

    QString targetFilePath = dir.filePath(QFileInfo(origin).fileName());

    QFile::remove(targetFilePath);

    if (!QFile::copy(origin, targetFilePath)) qWarning() << "Impossibile copiare il file:" << origin << "->" << targetFilePath;

}

QJsonObject trackToJson(const TrackFileSystemDto& track) {
    return QJsonObject{
        {"relativePath", track.relativePath},
        {"fileSize", track.fileSize},
        {"lastModified", track.lastModified}
    };
}

void testFullScan() {
    qDebug() << "inizio testfullscan";
    if (!StorageManager::instance().isMounted()) testStorage();
    
    QList<TrackFileSystemDto> files = LibraryScanner::scanAudioFiles(StorageManager::instance().musicPoint());
    QJsonArray jsonArray;
    
    int total = files.size();
    for (int i = 0; i < total; ++i) {
        jsonArray.append(trackToJson(files[i]));
        int percentage = (total > 0) ? static_cast<int>(((i + 1) * 100.0) / total) : 0;
        qDebug() << "FullScan JSON Progress:" << percentage << "% (" << (i + 1) << "/" << total << ")";
    }
    
    QJsonDocument doc(jsonArray);
    QFile file("../tmp/library.json");
    if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        file.write(doc.toJson(QJsonDocument::Indented));
        file.close();
    } else {
        qWarning() << "Impossibile aprire il file:" << file.errorString();
    }
    qDebug() << "fine testfullscan";
}

void testSmartScan() {
    qDebug() << "inizio testsmartscan";
    if (!StorageManager::instance().isMounted()) testStorage();
    if (!DatabaseManager::instance().openDatabase(StorageManager::instance().musicPoint() + "/" + "music_library.db")) {
        qDebug() << "testDb fallito";
    }
    
    // Callback lambda passata a smartScan per loggare il progresso
    auto logProgress = [](int current, int total) {
        int percentage = (total > 0) ? static_cast<int>((current * 100.0) / total) : 0;
        qDebug() << "SmartScan Progress:" << percentage << "% (" << current << "/" << total << ")";
    };

    ScanResultDto result = LibraryScanner::smartScan(StorageManager::instance().musicPoint(), logProgress);

    QJsonArray newTracks;
    for (const auto& f : result.newTracks) newTracks.append(trackToJson(f));

    QJsonArray modifiedTracks;
    for (const auto& f : result.modifiedTracks) modifiedTracks.append(trackToJson(f));

    QJsonArray deletedTracks;
    for (const auto& path : result.deletedTracks) deletedTracks.append(path);

    QJsonArray unchangedTracks;
    for (const auto& path : result.unchangedTracks) unchangedTracks.append(path);

    QJsonObject root{
        {"newTracks", newTracks},
        {"modifiedTracks", modifiedTracks},
        {"deletedTracks", deletedTracks},
        {"unchangedTracks", unchangedTracks}
    };

    QJsonDocument doc(root);

    QFile file("../tmp/scan_result.json");

    if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        file.write(doc.toJson(QJsonDocument::Indented));
        file.close();
    } else {
        qWarning() << "Impossibile aprire il file:" << file.errorString();
    }

    DatabaseManager::instance().closeDatabase();
    qDebug() << "fine testsmartscan";
}