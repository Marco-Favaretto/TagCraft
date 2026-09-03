# TagCraft
A desktop application, built with C++ and Qt in Arch Linux, designed to help organize and manage MP3 files.

The application allows users to catalog their music library, view and edit metadata tags, manage album artwork, and download audio using yt-dlp. The goal is to provide centralized tool for managing a local music collection, from downloading tracks to keeping their metadata and artwork properly organized, instead of relying on many python and shell scripts I've written throughout the years.

### Side Goal
Try to keep the project organized with GitHub Actions, Issues and Project.

---

## Version history
### version 0.1
Core application functionalities implemented:
- read tags
- edit single track and multiple tracks
- edit and read artwork
### version 0.2
The application runs and starts the UI. At the moment, the UI allows the user to navigate the music library following its hierarchy:

Artist → Album → Track
Genre → Track
The user can navigate back through the hierarchy by pressing Backspace. A search function is also available, allowing the user to quickly find tracks, albums, artists, or genres.
### version [0.3 - MVP](../../releases)
UI displays artwork and three buttons on the Tracks view:
1. Edit dialog (single track only): opens a dialog that allow user to edit: title, album name, artist name, genre, track number, year and artwork, or remove all tags
2. Open the localtion of the track or album in the file system
3. delete the location of the track or album selected from the file system

The library updates automatically after these changes are done.

---

# Installation Guide (Linux only right now)

## Option 1: Downloading Pre-built Binaries (Release)

1. Go to the [Releases](../../releases) page and download the latest archive (`TagCraft-vX.Y.Z-linux-x64.tar.gz`).
2. Ensure you have the required runtime dependencies installed:
   * `qt6-base` 
   * `taglib`
3. Extract the archive and run the application:
   ```bash
   tar -xzf TagCraft-v*.tar.gz
   cd package  # or the directory where it was extracted
   ./bin/TagCraft
   ```

## Option 2: Building from Source
### Prerequisites:
- C++20 compatible compiler (GCC 10+, Clang 11+)
- CMake 3.19+
- Qt 6.5+ (Core, Gui, Sql, Widgets)
- TagLib
### Build Steps
```bash
# 1. Clone the repository
git clone [https://github.com/YOUR_USERNAME/TagCraft.git](https://github.com/YOUR_USERNAME/TagCraft.git)
cd TagCraft
# 2. Configure the project
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
# 3. Build the executable
cmake --build build --config Release
# 4. Run the application
./build/TagCraft
```


---

## AI Usage in the Project (mostly Claude and Gemini)
* Assistance with problem-solving, reasoning, and identifying potential issues.
* Documentation, planning, and workflow organization.
* UI implementation and visual refinement, based on the baseline defined in `docs/requirements.md`. — because even the most confusing Picasso painting looks more organized than my best UI.
