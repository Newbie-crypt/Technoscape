cmake_minimum_required(VERSION 3.21)

project(CSII_Final_Project)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Multimedia)

set(SOURCE_FILES 
    src/main.cpp
    src/players.cpp
    src/wall.cpp
    src/furniture.cpp
    src/machine.cpp
    src/door.cpp
    src/trap.cpp
    src/weapon.cpp
    src/classes.cpp
    src/enemies.cpp
    src/menu_gameScene.cpp
    src/keyitem.cpp
    src/gameLevel.cpp
    src/pauseMenu.cpp
    src/gameOver.cpp
    src/levelOne.cpp
    src/levelTwo.cpp
    src/sideplayer.cpp
    include/players.hpp
    include/wall.hpp
    include/furniture.hpp
    include/machine.hpp
    include/door.hpp
    include/trap.hpp
    include/weapon.hpp
    include/classes.hpp
    include/enemies.hpp
    include/menu_gameScene.hpp
    include/leghitbox.hpp
    include/keyitem.hpp
    include/gameLevel.hpp
    include/pauseMenu.hpp
    include/gameOver.hpp
    include/levelOne.hpp
    include/levelTwo.hpp
    include/sideplayer.hpp
    )

qt_add_executable(main ${SOURCE_FILES} resources.qrc)

target_link_libraries(main PRIVATE Qt6::Widgets Qt6::Multimedia)
target_include_directories(main PRIVATE include)
file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})

qt_finalize_executable(main)
