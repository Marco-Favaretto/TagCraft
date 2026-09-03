#include "ui/editmodels/trackeditmodel.h"

#include "utils/formatter.h"
#include "dto/constants.h"

#include <QDateTime>

const QString TrackEditModel::KeyTitle = "title";
const QString TrackEditModel::KeyArtist = "artistName";
const QString TrackEditModel::KeyAlbum = "albumName";
const QString TrackEditModel::KeyGenre = "genreName";
const QString TrackEditModel::KeyTrackNumber = "trackNumber";
const QString TrackEditModel::KeyYear = "year";

TrackEditModel::TrackEditModel(const Track& track, LibraryController* library, QObject* parent)
    : AbstractEditModel(parent)
    , m_track(track)
    , library(library)
{}

ViewMode TrackEditModel::viewMode() const {
    return ViewMode::Tracks;
}

QString TrackEditModel::windowTitle() const {
    return QString("Edit Track");
}

QString TrackEditModel::artworkHash() const {
    return m_track.trackCoverHash() ? *m_track.trackCoverHash() : QString();
}

QList<EditField> TrackEditModel::fields() const {
    auto artistOpt = library->getArtistById(m_track.artistId());
    QString artistName = artistOpt.has_value() ? artistOpt->name() : Constants::DefaultValues::Artist;

    auto albumOpt = library->getAlbumById(m_track.albumId());
    QString albumName = albumOpt.has_value() ? albumOpt->title() : Constants::DefaultValues::Album;

    QString genreName = Constants::DefaultValues::Genre;
    auto genreIdOpt = m_track.genreId();

    if (genreIdOpt.has_value()) {
        auto genreOpt = library->getGenreById(*genreIdOpt);
        if (genreOpt.has_value()) {
            genreName = genreOpt->name();
        }
    }

    QList<EditField> result;

    EditField title;
    title.key = KeyTitle;
    title.label = "Title:";
    title.value = m_track.title();
    title.editable = true;
    title.type = EditField::Type::Text;
    result.append(title);

    EditField artist;
    artist.key = KeyArtist;
    artist.label = "Artist:";
    artist.value = artistName;
    artist.editable = true;
    artist.type = EditField::Type::Text;
    result.append(artist);

    EditField album;
    album.key = KeyAlbum;
    album.label = "Album:";
    album.value = albumName;
    album.editable = true;
    album.type = EditField::Type::Text;
    result.append(album);

    EditField genre;
    genre.key = KeyGenre;
    genre.label = "Genre:";
    genre.value = genreName;
    genre.editable = true;
    genre.type = EditField::Type::Text;
    result.append(genre);

    EditField trackNumber;
    trackNumber.key = KeyTrackNumber;
    trackNumber.label = "Track #:";
    trackNumber.value = m_track.trackNumber().value_or(0);
    trackNumber.editable = true;
    trackNumber.type = EditField::Type::Int;
    trackNumber.emptyIntValue = 0;
    result.append(trackNumber);

    EditField year;
    year.key = KeyYear;
    year.label = "Year:";
    year.value = m_track.year().value_or(0);
    year.editable = true;
    year.type = EditField::Type::Int;
    year.emptyIntValue = 0;
    result.append(year);

    EditField path;
    path.key = "relativePath";
    path.label = "Path:";
    path.value = m_track.relativePath();
    path.editable = false;
    path.type = EditField::Type::Text;
    result.append(path);

    EditField duration;
    duration.key = "durationSeconds";
    duration.label = "Duration:";
    duration.value = Formatter::formatDuration(m_track.durationSeconds().value_or(0));
    duration.editable = false;
    duration.type = EditField::Type::Text;
    result.append(duration);

    EditField fileSize;
    fileSize.key = "fileSize";
    fileSize.label = "File size:";
    fileSize.value = Formatter::formatFileSize(m_track.fileSize());
    fileSize.editable = false;
    fileSize.type = EditField::Type::Text;
    result.append(fileSize);

    EditField lastModified;
    lastModified.key = "lastModified";
    lastModified.label = "Last modified:";
    lastModified.value = Formatter::formatMtime(m_track.fileMtimeSecs());
    lastModified.editable = false;
    lastModified.type = EditField::Type::Text;
    result.append(lastModified);

    return result;
}

TrackDto TrackEditModel::buildDto(const QHash<QString, QVariant>& changedValues) const {
    TrackDto dto;

    auto artistOpt = library->getArtistById(m_track.artistId());
    QString artistName = artistOpt.has_value() ? artistOpt->name() : Constants::DefaultValues::Artist;

    auto albumOpt = library->getAlbumById(m_track.albumId());
    QString albumName = albumOpt.has_value() ? albumOpt->title() : Constants::DefaultValues::Album;

    QString genreName = Constants::DefaultValues::Genre;
    auto genreIdOpt = m_track.genreId();

    if (genreIdOpt.has_value()) {
        auto genreOpt = library->getGenreById(*genreIdOpt);
        if (genreOpt.has_value()) {
            genreName = genreOpt->name();
        }
    }

    dto.relativePath = m_track.relativePath();

    dto.title = changedValues.contains(KeyTitle)
        ? changedValues.value(KeyTitle).toString()
        : m_track.title();

    dto.artistName = changedValues.contains(KeyArtist)
        ? changedValues.value(KeyArtist).toString()
        : artistName;

    dto.albumName = changedValues.contains(KeyAlbum)
        ? changedValues.value(KeyAlbum).toString()
        : albumName;

    dto.genreName = changedValues.contains(KeyGenre)
        ? changedValues.value(KeyGenre).toString()
        : genreName;

    dto.trackNumber = changedValues.contains(KeyTrackNumber)
        ? changedValues.value(KeyTrackNumber).toInt()
        : m_track.trackNumber().value_or(0);

    dto.year = changedValues.contains(KeyYear)
        ? changedValues.value(KeyYear).toInt()
        : m_track.year().value_or(0);

    dto.durationSeconds = m_track.durationSeconds().value_or(0);
    dto.fileSize = m_track.fileSize();
    dto.lastModified = m_track.fileMtimeSecs();
    dto.coverHash = m_track.trackCoverHash() ? *m_track.trackCoverHash() : "NULL";

    return dto;
}
