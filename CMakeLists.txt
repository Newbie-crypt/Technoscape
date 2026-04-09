cmake_minimum_required(VERSION 3.21)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(SOURCE_FILES ./src/main.cpp ./src/players.cpp ./src/wall.cpp ./include/players.hpp)
project(app)
find_package(Qt6 REQUIRED COMPONENTS Widgets)

qt_add_executable(main ${SOURCE_FILES} resources.qrc)

target_link_libraries(main PRIVATE Qt6::Widgets)
target_include_directories(main PRIVATE include)

qt_finalize_executable(main)