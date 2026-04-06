cmake_minimum_required(VERSION 3.21)
set(SOURCE_FILES ./src/main.cpp ./src/players.cpp ./src/wall.cpp ./include/players.hpp)
set(CMAKE_AUTOMOC ON)
project(app)
find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(main ${SOURCE_FILES})

target_link_libraries(main PRIVATE Qt6::Widgets)
target_include_directories(main PRIVATE include)