cmake_minimum_required(VERSION 3.16)

project(ProjectVMenu VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(QML_IMPORT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/ui" CACHE STRING "" FORCE)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Core Quick LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Quick LinguistTools)
find_package(PkgConfig REQUIRED)
find_package(mimalloc REQUIRED)

pkg_check_modules (LIBCURL libcurl)

pkg_search_module(LIBCURL REQUIRED libcurl)

include_directories(${LIBCURL_INCLUDE_DIRS})

set(TS_LIST translations/ProjectVMenu_ru_RU.ts translations/ProjectVMenu_en_US.ts)

include_directories(
        ${PROJECT_SOURCE_DIR}/include
        ${PROJECT_SOURCE_DIR}/src
)

file(GLOB SRC
        "${PROJECT_SOURCE_DIR}/include/*.h"
		"${PROJECT_SOURCE_DIR}/include/*.hpp"
        "${PROJECT_SOURCE_DIR}/src/*.cpp"
		"${PROJECT_SOURCE_DIR}/src/*.c"
        )

set(PROJECT_SOURCES
        ${PROJECT_SOURCE_DIR}/src/main.cpp
        qml.qrc
)

qt_add_executable(ProjectVMenu
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
    ${SRC}
)

qt_add_translations(TARGETS ProjectVMenu TS_FILES ${TS_LIST} SOURCES ${PROJECT_SOURCES})

target_link_libraries(ProjectVMenu
  PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Quick ${LIBCURL_LIBRARIES})
target_link_libraries(ProjectVMenu PUBLIC mimalloc)

set_target_properties(ProjectVMenu PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS ProjectVMenu
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_import_qml_plugins(ProjectVMenu)
    qt_finalize_executable(ProjectVMenu)
endif()
