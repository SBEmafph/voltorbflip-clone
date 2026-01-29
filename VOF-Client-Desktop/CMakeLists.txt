cmake_minimum_required(VERSION 3.16)

project(VOF-Client-Desktop VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(
    QT NAMES Qt6 Qt5
    REQUIRED COMPONENTS
    Widgets LinguistTools
    Network
)

find_package(
    Qt${QT_VERSION_MAJOR}
    REQUIRED COMPONENTS
    Widgets LinguistTools
    Network
)

set(TS_FILES VOF-Client-Desktop_de_DE.ts)

set(PROJECT_SOURCES
    Main.cpp
        ${TS_FILES}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(VOF-Client-Desktop
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}

        Client.h Client.cpp
        Config.h Config.cpp
        Menu.ui Menu.h Menu.cpp
        Rules.ui Rules.h Rules.cpp
        Lobby.ui Lobby.h Lobby.cpp
        Match.h Match.cpp Match.ui
        Replay.h Replay.cpp Replay.ui
        Shop.h Shop.cpp Shop.ui
        Browser.h Browser.cpp Browser.ui
        Settings.h Settings.cpp Settings.ui
        resources.qrc
        CardButton.h
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET VOF-Client-Desktop APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation

    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    if(ANDROID)
        add_library(VOF-Client-Desktop SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(VOF-Client-Desktop
            ${PROJECT_SOURCES}
        )
    endif()

    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

target_link_libraries(
    VOF-Client-Desktop
    PRIVATE VOF-Network
    PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
    PRIVATE Qt${QT_VERSION_MAJOR}::Network
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.VOF-Client-Desktop)
endif()
set_target_properties(VOF-Client-Desktop PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS VOF-Client-Desktop
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(VOF-Client-Desktop)
endif()
