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