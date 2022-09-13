cmake_minimum_required(VERSION 3.20)
project(longmath_calculator VERSION 1.0.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

if(WIN32)
    set(CMAKE_PREFIX_PATH $ENV{QT_CMAKE_PATH_MSVC})
endif()

set(PROJECT_INCLUDE_DIRS
    "${CMAKE_SOURCE_DIR}/include"
    "${CMAKE_SOURCE_DIR}/lib/biginteger/single_include")

find_package(Qt6 COMPONENTS Core Gui Widgets REQUIRED)

add_subdirectory(src)
