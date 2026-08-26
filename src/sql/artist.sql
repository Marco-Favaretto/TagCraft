-- name: insert
INSERT INTO artist (
    name
) VALUES (
    :name
);

-- name: update
UPDATE artist
SET
    name = :name
WHERE id = :id;

-- name: findById
SELECT
    id,
    name
FROM artist
WHERE id = :id;

-- name: getAll
SELECT
    id,
    name
FROM artist;

-- name: deleteById
DELETE FROM artist 
WHERE id = :id;

-- name: getByName
SELECT id, name 
FROM artist 
WHERE LOWER(name) = LOWER(:name);

-- name: deleteOrphans
DELETE FROM artist AS a
WHERE NOT EXISTS (
    SELECT 1
    FROM track AS t
    WHERE t.artist_id = a.id
);