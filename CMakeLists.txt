cmake_minimum_required(VERSION 3.19)

project(TagCraft LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

set(CMAKE_AUTOUIC_SEARCH_PATHS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui
)

qt_add_executable(TagCraft
    WIN32 MACOSX_BUNDLE
    main.cpp
    src/sources/mainwindow.cpp
    src/headers/mainwindow.h
    src/ui/mainwindow.ui
)

target_include_directories(TagCraft
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/headers
)

target_link_libraries(TagCraft
    PRIVATE
        Qt6::Core
        Qt6::Widgets
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