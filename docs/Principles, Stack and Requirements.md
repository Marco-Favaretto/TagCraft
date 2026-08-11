# 1. Technology Stack
* **Language:** C++
* **GUI Framework:** Qt
* **Database:** SQLite
* **Database Interface:** Qt SQL
* **Markup and Styling:** HTML, CSS
* **Build System:** CMake
* **Audio Downloader:** `yt-dlp`
* **Metadata Library:** TagLib
* **Target Platform:** Arch Linux (Maybe W11 in future)

# 2. Data & Requirements Model
## 2.1 External Storage
The music library must live on an external physical storage device, such as an SSD or SD card. \
The storage must contain:
```text
/
├── Music/
│   ├── Artist/
│   │   └── Album/
│   │       ├── 01 - Track.mp3
│   │       ├── 02 - Track.mp3
│   │       └── cover.jpg
│   │
└── .music_app/
    ├── library.db
    └── covers/
```

Requirements:
* Identify the storage device through its partition UUID and/or filesystem label.
* Use `QStorageInfo` for storage detection.
* Do not rely on hardcoded absolute paths.
* Store library paths relative to the storage root.
* Resolve the library correctly regardless of the device's mount point.

## 2.2 SQLite Database
* The application must use an embedded SQLite database.
* Database location: `.music_app/library.db`.
* SQLite acts as an **index/cache**, not as the source of truth.
* MP3 files and their ID3 tags are the **Single Source of Truth**.
* The database must be reconstructable from the files stored on the external device.

### Entities
* `Track`
* `Artist`
* `Album`
* `Genre`

### Fallback Records
Missing metadata should be associated with fallback records:
* `Unknown Artist`
* `Unknown Album`
* `Unknown Genre`

## 2.3 Artwork
Artwork resolution priority:
1. Embedded artwork in the MP3 `APIC` frame.
2. `cover.jpg` in the album directory.
3. Application placeholder.

Rules:
* Shared album artwork must use the filename `cover.jpg`.
* Standalone tracks should store artwork inside the MP3 ID3 tags.
* Support extracting and embedding artwork.

### Artwork Cache
* Store resized thumbnails in `.music_app/covers/`.
* Identify cached images using a content hash.
* Store only the artwork hash/reference in SQLite.

## 2.4 File name patterns
1. Depending of which tags are valued, the file name pattern is: `{track_numer} - {artist name} - {album name} - {title}.mp3`
2. if the file name does not respect the tags' values, the application will ask if it can rename the file

# 3. Functional Requirements
## 3.1 Library Scanning
The application must:
* Scan configured directories for MP3 files.
* Detect new, modified, deleted, and missing files.
* Detect moved files where possible.
* Optionally detect duplicates.
* Display scan progress.

### Smart Scan
For files already indexed:
* Compare stored `mtime` and `file_size` with the current values.
* Read ID3 metadata with TagLib only when the file is new, modified, or missing valid database information.
* Update SQLite using batch transactions.
* Perform scanning in the background so the UI remains responsive.

## 3.2 Music Library Management
Users must be able to:
* Browse tracks grouped by artist, album, or genre.
* Search and filter the library.
* Sort tracks by title, artist, album, track number, or year.
* Open a file's location in the system file manager.
* Rename, move, and delete files.
* Refresh the library after external filesystem changes.

## 3.3 Metadata Management
The application must support reading and editing common ID3 metadata:
* Title
* Artist
* Album
* Album Artist
* Genre
* Year
* Track number
* artwork

Users must be able to:
* View and edit metadata.
* Save changes directly to the MP3.
* Preserve metadata that was not modified.
* Identify missing metadata.

## 3.4 Multi-Selection and Batch Operations
The library must support:
* Multiple individual selections.
* Range selection.
* Select all / deselect all.
* Selecting complete albums or multiple albums.

Batch operations should include:
* Editing metadata.
* Changing artist, album, genre, year, etc.
* Applying or removing artwork.
* Renaming files using the pattern defined in `#2.4`.
* Automatically extracting metadata from filenames.

Before applying a batch operation, the application must clearly indicate which fields and files will be affected.

## 3.5 Album and Track Ordering
* Tracks within an album must have an explicit order based primarily on their track number.
* If the track number is missing, fall back to alphanumeric filename/title ordering.
* Users must be able to reorder tracks.
* Reordering must update the track number in the ID3 metadata.
* If filenames follow the configured naming pattern, affected filenames must be updated as well.

Example:
```text
01 - Intro.mp3
02 - Song A.mp3
03 - Song B.mp3
04 - Outro.mp3
```
After reordering:
```text
01 - Intro.mp3
02 - Song B.mp3
03 - Song A.mp3
04 - Outro.mp3
```

## 3.6 Album Artwork Management
Users must be able to:
* View album and track artwork.
* Select an image from the filesystem.
* Replace or remove artwork.
* Extract artwork from an MP3.
* Embed artwork into an MP3.
* Apply artwork to multiple selected tracks.

## 3.7 File Management
Users must be able to:
* Open files in the system file manager.
* Rename files.
* Delete files.
* Perform these operations on multiple selected files where applicable.

The library index must be updated after filesystem operations. \
Destructive operations require confirmation.

## 3.8 MP3 Download
The application must integrate with `yt-dlp`. \
Users must be able to:
* Enter a supported URL.
* Download one or multiple tracks.
* Monitor download progress and logs.
* Cancel downloads.

Downloaded files must be automatically added to the library. \
Download failures must be reported without crashing the application.

# 4. UI Requirements
## 4.1 Main Layout
The application should provide three main areas:
```text
┌──────────────┬──────────────────────────┬──────────────────┐
│ Navigation   │ Track / Album List       │ Details          │
│              │                          │                  │
│ All Tracks   │                          │ Metadata         │
│ Artists      │                          │ Artwork          │
│ Albums       │                          │ Information      │
│ Genres       │                          │                  │
│ Downloads    │                          │                  │
│ Settings     │                          │                  │
└──────────────┴──────────────────────────┴──────────────────┘
```

### Navigation
The left sidebar should allow navigation through:
* All tracks
* Artists
* Albums
* Genres
* Downloads
* Settings

Artists, albums, and genres should work hierarchically:
```text
Artists
└── Artist
    └── Album
        └── Tracks
```

A search bar should be available from the library view.

### Center View
The central area displays the currently selected tracks or albums. \
The track list should support:
* Sorting
* Filtering
* Single and multi-selection
* Keyboard navigation
* Context menus
* Inline metadata editing where appropriate

Tracks without an album should be grouped under `Unknown Album`. The same principle applies to browsing by album and genre. 

### Details Sidebar
The right sidebar displays:
* Track/album information
* Metadata editor
* Artwork preview
* Actions relevant to the current selection

### Undecided artwork displays:
1. Null value on cover = default cover = square image with music note
2. Artist artwork is still to be decided. Possible options include an artist-specific logo, or a collage of album covers.
3. Unknown Album of a specific artist may have artist logo or default cover 
4. Genre, year and unknown album of unknown/various artists will use deafult cover, but sigle track cover will still be visualized when the tracks are displayed.

## 4.2 Metadata Editor
The metadata editor must:
* Clearly distinguish editable and read-only fields.
* Support single-track editing.
* Support batch editing.
* Indicate fields with different values across a multi-selection.
* Provide explicit **Save** and **Discard** actions.

## 4.3 Artwork Interaction
The artwork preview must support
* Click to view artwork.
* Changing artwork by choosing it from the file system
* Removing artwork.
* Applying artwork to multiple selected tracks.

## 4.4 Track Reordering
The album track list must support
* Immediate preview of the new order.
* Confirmation or undo before applying large changes where appropriate.

## 4.5 Modal Menus
Modal menus should provide relevant actions depending on the selected item.
Each item in the central box will have a small button to open a modal that: 
* For a track
    * Edit metadata
    * Edit artwork
    * Open file location
    * Rename
    * Move
    * Delete
    * Save/Drop changes
* For an album
    * Edit album metadata
    * Change artwork
    * Reorder tracks
    * Open album directory
    * Select all tracks
    * Save/Drop changes
* For multiple selected tracks
    * Batch edit metadata
    * Batch artwork operation
    * Move
    * Delete
    * Save/Drop changes

## 4.6 Feedback and Error Handling
The application must provide clear feedback for
* Library scanning.
* Metadata changes.
* Artwork changes.
* File operations.
* Downloads.
* Database operations.

Long-running operations must
* Display progress and logs where appropriate.
* Allow cancellation where possible.

Errors must identify the affected operation/file, be logged for debugging, and never crash the application.

# 5. Architecture Principles
* **MP3 files are the source of truth.**
* **ID3 tags are the source of truth for metadata.**
* **SQLite is an index/cache.**
* **The filesystem is the source of truth for file existence.**
* **All library paths are relative to the external storage root.**
* **The database must be rebuildable from the filesystem.**
* **The application should remain usable even when the external storage is temporarily unavailable.**

# 6. Other ideas that will be explored in the future

## 6.1 Portable Application
Explore storing and running the application directly from the external storage:
```text
storage root
├── Music/
├── .music_app/
└── app/
```
The main objective would be to make the **library and application portable between compatible machines**, independently of the storage device's mount point. \
Development would still be performed on the main machine. \
The main unresolved dependency is `yt-dlp`, which may remain a system requirement rather than being bundled with the application.

## 6.2 Drag and Drop Feature
1. Drag and Drop Artwork feature (sidebar and modal):
    * Dragging an image onto an album.
    * Dragging an image onto a track.
    * Dropping an image directly onto the artwork preview.
    * Replacing existing artwork through drag and drop.
2. Reordering tracks in list using drag and drop feature and visual indication of the drop position.

## 6.3 Sync with mp3
Add data in model to keep track of which mp3 file is in the mp3. 

## 6.4 Dark mode
Possibility to change with a toggle between light and dark mode, maybe using current system mode as default