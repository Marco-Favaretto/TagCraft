DELETE FROM track;
DELETE FROM album;
DELETE FROM genre;
DELETE FROM artist;

DELETE FROM sqlite_sequence
WHERE name IN ('track', 'album', 'genre', 'artist');

INSERT OR IGNORE INTO artist (id, name) VALUES (1, 'Unknown Artist');
INSERT OR IGNORE INTO album (id, title, artist_id) VALUES (1, 'Unknown Album', 1);
INSERT OR IGNORE INTO genre (id, name) VALUES (1, 'Unknown Genre');