-- 1. TABELLA ARTISTI
CREATE TABLE IF NOT EXISTS Artist (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL UNIQUE
);

-- 2. TABELLA ALBUM
CREATE TABLE IF NOT EXISTS Album (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    title TEXT NOT NULL,
    artist_id INTEGER NOT NULL DEFAULT 1,
    cover_cache_hash TEXT, -- Nome/Hash della thumbnail in cache (es. "a1b2c3d4.jpg")
    FOREIGN KEY (artist_id) REFERENCES Artist(id) ON DELETE RESTRICT,
    UNIQUE(title, artist_id) -- Previene duplicati di album con lo stesso nome da artisti diversi
);

-- 3. TABELLA GENERI
CREATE TABLE IF NOT EXISTS Genre (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL UNIQUE
);

-- 4. TABELLA TRACCE / FILE MP3
CREATE TABLE IF NOT EXISTS Track (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    title TEXT NOT NULL,
    artist_id INTEGER NOT NULL DEFAULT 1,
    album_id INTEGER NOT NULL DEFAULT 1,
    genre_id INTEGER DEFAULT 1,
    year INTEGER,
    track_number INTEGER,
    duration_seconds INTEGER,
    
    -- Dati per la gestione del file system su supporto esterno
    relative_path TEXT NOT NULL UNIQUE, -- es: "artisti/Pink Floyd/1977 Animals/01 - Pigs.mp3"
    file_mtime INTEGER NOT NULL,        -- Unix timestamp dell'ultima modifica (mtime)
    file_size INTEGER NOT NULL,         -- Dimensione in byte (per verificare l'integrità rapidamente)
    
    -- Copertina specifica per singola traccia (se diversa da quella dell'album)
    track_cover_hash TEXT,              -- NULL se usa la cover dell'album/default
    
    FOREIGN KEY (artist_id) REFERENCES Artist(id) ON DELETE RESTRICT,
    FOREIGN KEY (album_id) REFERENCES Album(id) ON DELETE RESTRICT,
    FOREIGN KEY (genre_id) REFERENCES Genre(id) ON DELETE RESTRICT
);

-- 5. INDICI PER PERFORMANCE UI
CREATE INDEX IF NOT EXISTS idx_track_path ON Track(relative_path);
CREATE INDEX IF NOT EXISTS idx_track_album ON Track(album_id);
CREATE INDEX IF NOT EXISTS idx_track_artist ON Track(artist_id);