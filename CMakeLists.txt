cmake_minimum_required(VERSION 3.5)

project(varnam-tools VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

# Locate govarnam library
find_library(VARNAM_LIBRARY NAMES govarnam)

# Include directories for Qt Widgets and govarnam
include_directories(${Qt${QT_VERSION_MAJOR}Widgets_INCLUDE_DIRS} /usr/local/include/govarnam)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        varnamcpp.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(varnam-tools
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    if(ANDROID)
        add_library(varnam-tools SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(varnam-tools
            ${PROJECT_SOURCES}



        )
    endif()
endif()

target_link_libraries(varnam-tools PRIVATE Qt${QT_VERSION_MAJOR}::Widgets ${VARNAM_LIBRARY})

include(GNUInstallDirs)
install(TARGETS varnam-tools
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(varnam-tools)
endif()
