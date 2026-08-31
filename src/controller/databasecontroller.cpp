#include "controller/databasecontroller.h"

#include "storage/storagemanager.h"
#include "db/transactionmanager.h"
#include "dao/albumdao.h"
#include "dao/artistdao.h"
#include "dao/genredao.h"
#include "dao/trackdao.h"
#include "dto/trackdto.h"
#include "utils/tagmapper.h"

DatabaseController::DatabaseController(QObject* parent) : QObject(parent) {}

bool DatabaseController::insertNewTracks(const QList<TrackFileSystemDto>& list) {
    auto& db = DatabaseManager::instance();
    TransactionManager transaction(db);
    if (!transaction.isStarted()) return false; // Impossibile avviare la transazione

    int total = list.size();
    int current = 0;
    for (const auto& t : list) {
        if (!insertTrackInternal(t)) return false; // metodo helper di inserimento (converte TrackFileSystemDto in entity Track e poi dao.insert con check per nuovi album, artist e genre)
        emit persistProgress(++current, total);
    }

    return transaction.commit();
}

bool DatabaseController::updateNewTracks(const QList<TrackFileSystemDto>& list) {
    auto& db = DatabaseManager::instance();
    TransactionManager transaction(db);
    if (!transaction.isStarted()) return false; // Impossibile avviare la transazione
    
    int total = list.size();
    int current = 0;

    // for(auto t : list) getTrackByRelativePath (ma se è cambiato quello?) se cambiato path -> vecchio path in deleted e nuovo path in new
    // entity dao.updateTrack() ed eventualmente artist, genre, album
    for (const auto& t : list) {
        if (!updateTrackInternal(t)) return false;
        emit persistProgress(++current, total);
    }

    return transaction.commit();
}

bool DatabaseController::deleteNewTracks(const QList<QString>& list) {
    auto& db = DatabaseManager::instance();
    TransactionManager transaction(db);
    if (!transaction.isStarted()) return false; // Impossibile avviare la transazione

    // for(auto t : list) delete track by relPath
    for(const auto& t : list) {
        auto trackOpt = TrackDao::findByRelativePath(t); 
        if (!trackOpt) return false;
        if(!TrackDao::deleteById(trackOpt->id())) return false;
    }

    return transaction.commit();
}

bool DatabaseController::deleteOrphans() {
    bool genreOrph = GenreDao::deleteOrphans();
    bool albumOrph = AlbumDao::deleteOrphans();
    bool artistOrph = ArtistDao::deleteOrphans();

    return genreOrph && albumOrph && artistOrph;
}

std::optional<Track> DatabaseController::insertTrackInternal(const TrackFileSystemDto& fsDto) {
    QString absolutePath = StorageManager::instance().toAbsolutePath(fsDto.relativePath);
    TrackDto tagDto = TagMapper::fileToDto(absolutePath, fsDto.relativePath);

    int artistId = resolveArtistId(tagDto.artistName);
    int albumId = resolveAlbumId(tagDto.albumName, artistId, tagDto.year);
    int genreId = resolveGenreId(tagDto.genreName);

    Track track;
    track.setTitle(tagDto.title);
    track.setArtistId(artistId);
    track.setAlbumId(albumId);
    track.setGenreId(genreId);
    track.setYear(tagDto.year);
    track.setTrackNumber(tagDto.trackNumber);
    track.setRelativePath(fsDto.relativePath);
    track.setFileMtimeSecs(fsDto.lastModified);
    track.setFileSize(fsDto.fileSize);

    if (!TrackDao::insert(track)) return std::nullopt;
    return track;
}

bool DatabaseController::updateTrackInternal(const TrackFileSystemDto& fsDto) {
    auto trackOpt = TrackDao::findByRelativePath(fsDto.relativePath);
    if (!trackOpt) return false;
    Track track = *trackOpt;

    QString absolutePath = StorageManager::instance().toAbsolutePath(fsDto.relativePath);
    TrackDto tagDto = TagMapper::fileToDto(absolutePath, fsDto.relativePath);

    int artistId = resolveArtistId(tagDto.artistName);
    int albumId = resolveAlbumId(tagDto.albumName, artistId, tagDto.year);
    int genreId = resolveGenreId(tagDto.genreName);

    track.setTitle(tagDto.title);
    track.setArtistId(artistId);
    track.setAlbumId(albumId);
    track.setGenreId(genreId);
    track.setYear(tagDto.year);
    track.setTrackNumber(tagDto.trackNumber);
    track.setFileMtimeSecs(fsDto.lastModified);
    track.setFileSize(fsDto.fileSize);

    return TrackDao::update(track);
}

bool DatabaseController::resetDb() {
    auto& db = DatabaseManager::instance();
    TransactionManager transaction(db);
    if (!transaction.isStarted()) return false;

    if (!db.resetDb()) {
        qCritical() << "Errore durante il reset del database";
        return false;
    }
    
    return transaction.commit();
}

bool DatabaseController::updateTrackCoverHash(const QString& relativePath, const QString& hash) {
    auto trackOpt = TrackDao::findByRelativePath(relativePath);
    if (!trackOpt) return false;

    return TrackDao::updateCover(trackOpt->id(), hash);
}

int DatabaseController::resolveArtistId(const QString& name) {
    if (name.trimmed().isEmpty()) return 1; // Unknown Artist
    auto artistOpt = ArtistDao::getOrCreate(name);
    return artistOpt ? artistOpt->id() : 1;
}

int DatabaseController::resolveAlbumId(const QString& title, int artistId, std::optional<int> year) {
    if (title.trimmed().isEmpty()) return 1; // Unknown Album
    auto albumOpt = AlbumDao::getOrCreate(title, artistId, year);
    return albumOpt ? albumOpt->id() : 1;
}

int DatabaseController::resolveGenreId(const QString& name) {
    if (name.trimmed().isEmpty()) return 1; // Unknown Genre
    auto genreOpt = GenreDao::getOrCreate(name);
    return genreOpt ? genreOpt->id() : 1;
}