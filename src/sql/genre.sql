-- name: insert
INSERT INTO genre (
    name
) VALUES (
    :name
);

-- name: update
UPDATE genre
SET
    name = :name
WHERE id = :id;

-- name: findById
SELECT
    id,
    name
FROM genre
WHERE id = :id;

-- name: getAll
SELECT
    id,
    name
FROM genre;