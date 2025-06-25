cmake_minimum_required(VERSION 3.16)

project(ProjectVMenu VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(QML_IMPORT_PATH ${PROJECT_SOURCE_DIR})
find_package(QT NAMES Qt5 REQUIRED COMPONENTS Core Quick)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Quick)
find_package(PkgConfig REQUIRED)
pkg_check_modules (JEMALLOC jemalloc)

pkg_search_module(JEMALLOC REQUIRED jemalloc)
include_directories(${JEMALLOC_INCLUDE_DIRS})




pkg_check_modules (LIBXML libxml-2.0)

pkg_search_module(LIBXML REQUIRED libxml-2.0)
include_directories(${LIBXML_INCLUDE_DIRS})




pkg_check_modules (LIBCURL libcurl)

pkg_search_module(LIBCURL REQUIRED libcurl)
include_directories(${LIBCURL_INCLUDE_DIRS})


set(PROJECT_SOURCES
        main.cpp
        qml.qrc
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(ProjectVMenu
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET ProjectVMenu APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(ProjectVMenu SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(ProjectVMenu
          ${PROJECT_SOURCES}
          imageparser.h
        )
    endif()
endif()

target_link_libraries(ProjectVMenu
  PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Quick ${JEMALLOC_LIBRARIES} ${LIBXML_LIBRARIES} ${LIBCURL_LIBRARIES})

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.ProjectVMenu)
endif()
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
