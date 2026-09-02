-- name: insert
INSERT INTO album (title, artist_id, year, relative_path, cover_cache_hash)
VALUES (:title, :artist_id, :year, :relative_path, :cover_cache_hash);

-- name: update
UPDATE album
SET
    title = :title,
    artist_id = :artist_id,
    year = :year,
    relative_path = :relative_path,
    cover_cache_hash = :cover_cache_hash
WHERE id = :id;

-- name: findById
SELECT
    id,
    title,
    artist_id,
    year,
    relative_path,
    cover_cache_hash
FROM album
WHERE id = :id;

-- name: getAll
SELECT
    id,
    title,
    artist_id,
    year,
    relative_path,
    cover_cache_hash
FROM album;

-- name: deleteById
DELETE FROM album 
WHERE id = :id;

-- name: getByTitleAndArtist
SELECT id, title, artist_id, year, relative_path, cover_cache_hash 
FROM album
WHERE LOWER(title) = LOWER(:title) AND artist_id = :artist_id;

-- name: deleteOrphans
DELETE FROM album AS a
WHERE NOT EXISTS (
    SELECT 1
    FROM track AS t
    WHERE t.album_id = a.id
);

-- name: getByArtistId
SELECT id, title, artist_id, year, relative_path, cover_cache_hash 
FROM album 
WHERE artist_id = :artist_id 
ORDER BY year DESC, title ASC;

-- name: getByTitleAndRelativePath
SELECT id, title, artist_id, year, relative_path, cover_cache_hash 
FROM album 
WHERE LOWER(title) = LOWER(:title) AND relative_path = :relative_path;

-- name: searchByKeyword
SELECT id, title, artist_id, year, relative_path, cover_cache_hash 
FROM album 
WHERE LOWER(title) LIKE LOWER(:keyword) 
ORDER BY title ASC;

-- name: drop
DELETE FROM album;

-- name: updateCoverAlbum
UPDATE album
SET cover_cache_hash = :cover_cache_hash
where id = :id;