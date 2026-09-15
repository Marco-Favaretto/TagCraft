#include "ui/editmodels/albumeditmodel.h"

#include "dto/constants.h"

const QString AlbumEditModel::KeyTitle = "title";
const QString AlbumEditModel::KeyArtist = "artistName";
const QString AlbumEditModel::KeyGenre = "genreName";
const QString AlbumEditModel::KeyTrackNumbers = "trackNumbers";

AlbumEditModel::AlbumEditModel(const Album& album, LibraryController* library, QObject* parent)
    : AbstractEditModel(parent)
    , m_album(album)
    , m_library(library)
{}

ViewMode AlbumEditModel::viewMode() const {
    return ViewMode::Albums;
}

QString AlbumEditModel::windowTitle() const {
    return QString("Edit Album");
}

QString AlbumEditModel::artworkHash() const {
    return m_album.coverCacheHash() ? *m_album.coverCacheHash() : QString();
}

QList<EditField> AlbumEditModel::fields() const {
    QList<EditField> result;

    auto artistOpt = m_library ? m_library->getArtistById(m_album.artistId()) : std::nullopt;
    auto genreOpt = m_library ? m_library->getGenreById(m_album.genreId()) : std::nullopt;

    EditField title;
    title.key = KeyTitle;
    title.label = "Title:";
    title.value = m_album.title();
    title.editable = true;
    title.type = EditField::Type::Text;
    result.append(title);

    EditField artist;
    artist.key = KeyArtist;
    artist.label = "Artist:";
    artist.value = artistOpt ? artistOpt->name() : QString("Unknown Artist");
    artist.editable = true;
    artist.type = EditField::Type::Text;
    result.append(artist);

    EditField genre;
    genre.key = KeyGenre;
    genre.label = "Genre:";
    genre.value = genreOpt ? genreOpt->name() : QString("Unknown Genre");
    genre.editable = true;
    genre.type = EditField::Type::Text;
    result.append(genre);

    EditField path;
    path.key = "relativePath";
    path.label = "Path:";
    path.value = m_album.relativePath() == Constants::DefaultValues::AlbumPath
        ? QString("-")
        : m_album.relativePath();
    path.editable = false;
    path.type = EditField::Type::Text;
    result.append(path);

    return result;
}

QHash<QString, QVariant> AlbumEditModel::buildResult(const QHash<QString, QVariant>& changedValues) const {
    QHash<QString, QVariant> result;

    result.insert("id", m_album.id());
    result.insert(KeyTitle, changedValues.contains(KeyTitle) ? changedValues.value(KeyTitle) : QVariant(m_album.title()));
    
    if (changedValues.contains(KeyArtist)) {
        result.insert(KeyArtist, changedValues.value(KeyArtist));
    } else {
        auto artistOpt = m_library ? m_library->getArtistById(m_album.artistId()) : std::nullopt;
        result.insert(KeyArtist, artistOpt ? artistOpt->name() : QString("Unknown Artist"));
    }

    if (changedValues.contains(KeyGenre)) {
        result.insert(KeyGenre, changedValues.value(KeyGenre));
    } else {
        auto genreOpt = m_library ? m_library->getGenreById(m_album.genreId()) : std::nullopt;
        result.insert(KeyGenre, genreOpt ? genreOpt->name() : QString("Unknown Genre"));
    }

    return result;
}

QList<Track> AlbumEditModel::tracks() const {
    if (!m_library) return {};
    return m_library->getTracksByAlbum(m_album.id());
}
