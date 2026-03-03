cmake_minimum_required(VERSION 3.16)

project(VOF-Server-CLI LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(
  QT NAMES Qt6 Qt5
  REQUIRED COMPONENTS
  Core
  Network
)

find_package(
  Qt${QT_VERSION_MAJOR}
  REQUIRED COMPONENTS
  Core
  Network
)

add_executable(VOF-Server-CLI
  Main.cpp
  VoltOrbFlipServer.cpp
  VoltOrbFlipServer.h
  PlayerProfile.h
)

target_include_directories(
  VOF-Server-CLI INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(
  VOF-Server-CLI
  PUBLIC VOF-Network
  Qt${QT_VERSION_MAJOR}::Core
  Qt${QT_VERSION_MAJOR}::Network
)

include(GNUInstallDirs)
install(TARGETS VOF-Server-CLI
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
