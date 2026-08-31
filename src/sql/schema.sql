-- 1. TABELLA ARTISTI
CREATE TABLE IF NOT EXISTS artist (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL UNIQUE
);

-- 2. TABELLA ALBUM
CREATE TABLE IF NOT EXISTS album (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    title TEXT NOT NULL,
    artist_id INTEGER NOT NULL DEFAULT 1,
    year INTEGER,
    cover_cache_hash TEXT,
    FOREIGN KEY (artist_id) REFERENCES artist(id) ON DELETE RESTRICT,
    UNIQUE(title, artist_id)
);

-- 3. TABELLA GENERI
CREATE TABLE IF NOT EXISTS genre (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL UNIQUE
);

-- 4. TABELLA TRACCE
CREATE TABLE IF NOT EXISTS track (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    title TEXT NOT NULL,
    artist_id INTEGER NOT NULL DEFAULT 1,
    album_id INTEGER NOT NULL DEFAULT 1,
    genre_id INTEGER DEFAULT 1,
    year INTEGER,
    track_number INTEGER,
    duration_seconds INTEGER,
    
    relative_path TEXT NOT NULL UNIQUE,
    file_mtime INTEGER NOT NULL,
    file_size INTEGER NOT NULL,
    track_cover_hash TEXT,
    
    FOREIGN KEY (artist_id) REFERENCES artist(id) ON DELETE RESTRICT,
    FOREIGN KEY (album_id) REFERENCES album(id) ON DELETE RESTRICT,
    FOREIGN KEY (genre_id) REFERENCES genre(id) ON DELETE RESTRICT
);

-- 5. INDICI PER PERFORMANCE
CREATE INDEX IF NOT EXISTS idx_track_path ON track(relative_path);
CREATE INDEX IF NOT EXISTS idx_track_album ON track(album_id);
CREATE INDEX IF NOT EXISTS idx_track_artist ON track(artist_id);