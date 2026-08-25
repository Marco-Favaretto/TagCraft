-- name: insert
INSERT INTO album (
    title,
    artist_id,
    cover_cache_hash
) VALUES (
    :title,
    :artist_id,
    :cover_cache_hash
);

-- name: update
UPDATE album
SET
    title = :title,
    artist_id = :artist_id,
    cover_cache_hash = :cover_cache_hash
WHERE id = :id;

-- name: findById
SELECT
    id,
    title,
    artist_id,
    cover_cache_hash
FROM album
WHERE id = :id;

-- name: getAll
SELECT
    id,
    title,
    artist_id,
    cover_cache_hash
FROM album;

-- name: deleteById
DELETE FROM album 
WHERE id = :id;

-- name: getByTitleAndArtist
SELECT id, title, artist_id, year, cover_hash 
FROM album 
WHERE LOWER(title) = LOWER(:title) AND artist_id = :artist_id;