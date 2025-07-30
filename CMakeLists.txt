cmake_minimum_required(VERSION 3.16)

project(DriverSentinel VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(CMAKE_PREFIX_PATH "C:/Program Files (x86)/LIEF/lib/cmake")
find_package(LIEF REQUIRED COMPONENTS STATIC)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
)
set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_SOURCE_DIR})

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(DriverSentinel
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        include/data/driverinfo.h
        include/model/drivermodel.h
        src/model/drivermodel.cpp
        include/controller/drivercontroller.h

        include/toolbar/drivertoolbar.h
        src/toolbar/drivertoolbar.cpp
        include/controller/drivercontroller.h include/data/driverinfo.h include/model/drivermodel.h include/toolbar/drivertoolbar.h src/controller/drivercontroller.cpp src/model/drivermodel.cpp src/toolbar/drivertoolbar.cpp
        include/data/generalinfo.h
        include/controller/generalcontroller.h
        src/controller/generalcontroller.cpp
        include/data/sectioninfo.h
        include/model/sectioninfomodel.h
        src/model/sectioninfomodel.cpp
        include/controller/sectioninfocontroller.h
        src/controller/sectioninfocontroller.cpp
        include/data/certificateinfo.h
        include/model/certificatemodel.h
        src/model/certificatemodel.cpp
        include/controller/certificatecontroller.h
        src/controller/certificatecontroller.cpp
        include/utils/peutils.h
        src/utils/peutils.cpp
        include/data/treeimportsitem.h
        src/data/treeimportsitem.cpp
        include/model/treeimportsmodel.h
        src/model/treeimportsmodel.cpp
        include/controller/treeimportscontroller.h
        src/controller/treeimportscontroller.cpp
        include/data/functioninfo.h
        include/model/functioninfomodel.h
        src/model/functioninfomodel.cpp



    )
target_include_directories(DriverSentinel
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/external
)
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET DriverSentinel APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(DriverSentinel SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(DriverSentinel
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(DriverSentinel PRIVATE Qt${QT_VERSION_MAJOR}::Widgets LIEF::LIEF)
target_link_libraries(DriverSentinel PRIVATE Version)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.DriverSentinel)
endif()
set_target_properties(DriverSentinel PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS DriverSentinel
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(DriverSentinel)
endif()
