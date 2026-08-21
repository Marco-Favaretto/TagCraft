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

-- name: filesState
SELECT 
    relative_path, 
    file_mtime, 
    file_size 
FROM track;