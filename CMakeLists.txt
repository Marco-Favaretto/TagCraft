cmake_minimum_required(VERSION 3.19)

project(TagCraft LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_AUTORCC ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Gui Sql Widgets)

# Gestione TagLib (su Arch Linux la configurazione CMake/PkgConfig è nativa)
find_package(TagLib NO_MODULE QUIET)

if(NOT TagLib_FOUND)
    # Fallback su PkgConfig per distribuzioni come Ubuntu/Debian
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(TagLib REQUIRED IMPORTED_TARGET taglib)
    
    # Crea il target ALIAS TagLib::TagLib se non esiste già
    if(NOT TARGET TagLib::TagLib)
        add_library(TagLib::TagLib ALIAS PkgConfig::TagLib)
    endif()
endif()

qt_standard_project_setup()

set(CMAKE_AUTOUIC_SEARCH_PATHS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui
)

qt_add_executable(TagCraft
    WIN32
    MACOSX_BUNDLE

    resources.qrc

    src/main.cpp

    src/model/artist.h
    src/model/artist.cpp
    src/model/album.h
    src/model/album.cpp
    src/model/genre.h
    src/model/genre.cpp
    src/model/track.h
    src/model/track.cpp

    src/dao/trackdao.h
    src/dao/trackdao.cpp
    src/dao/albumdao.h
    src/dao/albumdao.cpp
    src/dao/artistdao.h
    src/dao/artistdao.cpp
    src/dao/genredao.h
    src/dao/genredao.cpp

    src/ui/mainwindow.cpp
    src/ui/mainwindow.h
    src/ui/mainwindow.ui

    src/db/databasemanager.h
    src/db/databasemanager.cpp
    src/db/sqlparser.cpp
    src/db/sqlparser.h
    src/db/entitymapper.cpp
    src/db/entitymapper.h
    src/db/sqlexecutor.h
    src/db/sqlexecutor.cpp
    
    src/utils/dbutils.h
    src/utils/tagmapper.h
    src/utils/tagmapper.cpp

    src/storage/storagemanager.h
    src/storage/storagemanager.cpp
    src/storage/libraryscanner.h
    src/storage/libraryscanner.cpp
        
    src/dto/trackdto.h
    src/dto/trackfilesystemdto.h
)

target_include_directories(TagCraft
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${TagLib_INCLUDE_DIRS}
)

# Link delle librerie Qt e TagLib
target_link_libraries(TagCraft
    PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Sql
        Qt6::Widgets
        TagLib::TagLib
)

install(TARGETS TagCraft
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET TagCraft
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)

install(SCRIPT ${deploy_script})