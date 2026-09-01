-- name: insert
INSERT INTO track (
    title, 
    artist_id, 
    album_id, 
    genre_id, 
    year, 
    track_number, 
    duration_seconds, 
    relative_path, 
    file_mtime, 
    file_size, 
    track_cover_hash
) VALUES (
    :title, 
    :artist_id, 
    :album_id, 
    :genre_id, 
    :year, 
    :track_number, 
    :duration_seconds, 
    :relative_path, 
    :file_mtime, 
    :file_size, 
    :track_cover_hash
);

-- name: update
UPDATE track
SET
    title = :title,
    artist_id = :artist_id,
    album_id = :album_id,
    genre_id = :genre_id,
    year = :year,
    track_number = :track_number,
    duration_seconds = :duration_seconds,
    relative_path = :relative_path,
    file_mtime = :file_mtime,
    file_size = :file_size,
    track_cover_hash = :track_cover_hash
WHERE id = :id;

-- name: findById
SELECT 
    id, 
    title, 
    artist_id, 
    album_id, 
    genre_id, 
    year, 
    track_number, 
    duration_seconds, 
    relative_path, 
    file_mtime, 
    file_size, 
    track_cover_hash 
FROM track 
WHERE id = :id;

-- name: findByRelativePath
SELECT 
    id, 
    title, 
    artist_id, 
    album_id, 
    genre_id, 
    year, 
    track_number, 
    duration_seconds, 
    relative_path, 
    file_mtime, 
    file_size, 
    track_cover_hash 
FROM track 
WHERE relative_path = :relative_path;

-- name: getAll
SELECT 
    id, 
    title, 
    artist_id, 
    album_id, 
    genre_id, 
    year, 
    track_number, 
    duration_seconds, 
    relative_path, 
    file_mtime, 
    file_size, 
    track_cover_hash 
FROM track;

-- name: getUnknownAlbumOfArtist
SELECT 
    id, 
    title, 
    artist_id, 
    album_id, 
    genre_id, 
    year, 
    track_number, 
    duration_seconds, 
    relative_path, 
    file_mtime, 
    file_size, 
    track_cover_hash 
FROM track
where album_id = 1 AND artist_id = :artist_id;

-- name: filesState
SELECT 
    relative_path, 
    file_mtime, 
    file_size 
FROM track;

-- name: deleteById
DELETE FROM track 
WHERE id = :id;

-- name: deleteByRelativePath
DELETE FROM track WHERE relative_path = :relative_path;

-- name: getByAlbumId
SELECT id, title, artist_id, album_id, genre_id, year, track_number, duration_seconds, relative_path, file_mtime, file_size, track_cover_hash 
FROM track 
WHERE album_id = :album_id 
ORDER BY track_number ASC, title ASC;

-- name: getByArtistId
SELECT id, title, artist_id, album_id, genre_id, year, track_number, duration_seconds, relative_path, file_mtime, file_size, track_cover_hash 
FROM track 
WHERE artist_id = :artist_id 
ORDER BY album_id ASC, track_number ASC;

-- name: getByGenreId
SELECT id, title, artist_id, album_id, genre_id, year, track_number, duration_seconds, relative_path, file_mtime, file_size, track_cover_hash 
FROM track 
WHERE genre_id = :genre_id 
ORDER BY title ASC;

-- name: searchByKeyword
SELECT id, title, artist_id, album_id, genre_id, year, track_number, duration_seconds, relative_path, file_mtime, file_size, track_cover_hash 
FROM track 
WHERE LOWER(title) LIKE LOWER(:keyword)
ORDER BY title ASC;

-- name: updateCover
UPDATE track
SET
    track_cover_hash = :track_cover_hash
WHERE id = :id;

-- name: drop
DELETE FROM track;