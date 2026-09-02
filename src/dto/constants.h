#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QString>

namespace Constants::DefaultValues {
    inline constexpr int AlbumId = 1;
    inline const QString Album = "Unknown Album";
    inline const QString AlbumPath = "__unknown__";
    inline constexpr int ArtistId = 1;
    inline const QString Artist = "Unknown Artist";
    inline constexpr int GenreId = 1;
    inline const QString Genre = "Unknown Genre";
}

namespace Constants::Paths {
    inline const QString MusicDir = "Music";
    inline const QString AppDataDir = ".music_app";
    inline const QString DatabaseFileName = "music_library.db";
    inline const QString CoversDir = "covers";
    inline const QString CoverImageFileName = "cover.jpg";

}

namespace Constants::Artwork {
    inline constexpr int ThumbnailMaxWidth = 500;
    inline constexpr int ThumbnailMaxHeight = 500;
    inline constexpr int JpegQuality = 90;
    inline const QString TrackArtwork = ":/icons/default-track";
    inline const QString AlbumArtwork = ":/icons/default-album";
}

namespace Constants::FileFilters {
    inline const QString Mp3Extension = "*.mp3";
}

namespace Constants::Sql {
    inline const QString Schema = ":/sql/schema.sql";
    inline const QString StartValues = ":/sql/startvalues.sql";
    inline const QString Track = ":/sql/track.sql";
    inline const QString Artist = ":/sql/artist.sql";
    inline const QString Album = ":/sql/album.sql";
    inline const QString Genre = ":/sql/genre.sql";
    inline const QString ResetDb = ":/sql/resetdb.sql";
}


#endif // CONSTANTS_H