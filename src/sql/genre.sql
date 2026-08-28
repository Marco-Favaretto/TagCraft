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

-- name: deleteById
DELETE FROM genre 
WHERE id = :id;

-- name: getByName
SELECT id, name 
FROM genre 
WHERE LOWER(name) = LOWER(:name);

-- name: deleteOrphans
DELETE FROM genre AS g
WHERE NOT EXISTS (
    SELECT 1
    FROM track AS t
    WHERE t.genre_id = g.id
);

-- name: searchByKeyword
SELECT id, name FROM genre WHERE LOWER(name) LIKE LOWER(:keyword) ORDER BY name ASC;

-- name: drop
DELETE FROM genre;