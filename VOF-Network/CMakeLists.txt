cmake_minimum_required(VERSION 3.16)

project(VOF-Network LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(
  QT NAMES Qt6 Qt5
  REQUIRED COMPONENTS
  Core Network
)

find_package(
  Qt${QT_VERSION_MAJOR}
  REQUIRED COMPONENTS
  Core Network
)

add_library(VOF-Network STATIC
  NetworkObserver.cpp
  NetworkObserver.h

  IObserver.h
  GlobalDefines.h
  GlobalDefines.cpp
  Gamelogic.cpp
  Gamelogic.h
)

target_include_directories(
  VOF-Network INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(
  VOF-Network PRIVATE
  Qt${QT_VERSION_MAJOR}::Core
  Qt${QT_VERSION_MAJOR}::Network
)

target_compile_definitions(
  VOF-Network
  PRIVATE VOF_NETWORK_LIBRARY
)
