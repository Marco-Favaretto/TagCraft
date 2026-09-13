#include "ui/editmodels/batchtrackeditmodel.h"
#include "ui/editmodels/trackeditmodel.h"

const QString BatchTrackEditModel::MixedValuesPlaceholder = "< valori multipli >";

BatchTrackEditModel::BatchTrackEditModel(const QList<Track>& tracks, LibraryController* library, QObject* parent)
    : AbstractEditModel(parent)
    , m_tracks(tracks)
    , m_library(library)
{}

QString BatchTrackEditModel::windowTitle() const {
    return QString("Edit %1 Tracks").arg(m_tracks.size());
}

QString BatchTrackEditModel::resolvedArtistName(const Track& t) const {
    auto artistOpt = m_library ? m_library->getArtistById(t.artistId()) : std::nullopt;
    return artistOpt ? artistOpt->name() : QString("Unknown Artist");
}

QString BatchTrackEditModel::resolvedAlbumName(const Track& t) const {
    auto albumOpt = m_library ? m_library->getAlbumById(t.albumId()) : std::nullopt;
    return albumOpt ? albumOpt->title() : QString("Unknown Album");
}

QString BatchTrackEditModel::resolvedGenreName(const Track& t) const {
    auto genreOpt = m_library ? m_library->getGenreById(t.genreId().value_or(1)) : std::nullopt;
    return genreOpt ? genreOpt->name() : QString("Unknown Genre");
}

QString BatchTrackEditModel::commonValue(std::function<QString(const Track&)> resolver) const {
    if (m_tracks.isEmpty()) return QString();

    const QString first = resolver(m_tracks.first());
    for (const Track& t : m_tracks) {
        if (resolver(t) != first) return MixedValuesPlaceholder;
    }
    return first;
}

QString BatchTrackEditModel::artworkHash() const {
    if (m_tracks.isEmpty()) return QString();

    const QString first = m_tracks.first().trackCoverHash() ? *m_tracks.first().trackCoverHash() : QString();
    for (const Track& t : m_tracks) {
        const QString hash = t.trackCoverHash() ? *t.trackCoverHash() : QString();
        if (hash != first) return QString(); // mixed -> placeholder di default nel dialog
    }
    return first;
}

QList<EditField> BatchTrackEditModel::fields() const {
    QList<EditField> result;

    EditField artist;
    artist.key = TrackEditModel::KeyArtist;
    artist.label = "Artist:";
    artist.value = commonValue([this](const Track& t) { return resolvedArtistName(t); });
    artist.editable = true;
    artist.type = EditField::Type::Text;
    result.append(artist);

    EditField album;
    album.key = TrackEditModel::KeyAlbum;
    album.label = "Album:";
    album.value = commonValue([this](const Track& t) { return resolvedAlbumName(t); });
    album.editable = true;
    album.type = EditField::Type::Text;
    result.append(album);

    EditField genre;
    genre.key = TrackEditModel::KeyGenre;
    genre.label = "Genre:";
    genre.value = commonValue([this](const Track& t) { return resolvedGenreName(t); });
    genre.editable = true;
    genre.type = EditField::Type::Text;
    result.append(genre);

    return result;
}
