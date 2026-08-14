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