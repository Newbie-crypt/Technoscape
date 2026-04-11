cmake_minimum_required(VERSION 3.16)

project(CSII_Final_Project)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Multimedia)

add_executable(main
    src/main.cpp
    src/players.cpp
    src/wall.cpp
    src/furniture.cpp
    src/machine.cpp
    src/door.cpp
    src/trap.cpp
    include/players.hpp
    include/wall.hpp
    include/furniture.hpp
    include/machine.hpp
    include/door.hpp
    include/trap.hpp
)

target_include_directories(main PRIVATE include)

target_link_libraries(main PRIVATE Qt6::Widgets Qt6::Multimedia)

file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})