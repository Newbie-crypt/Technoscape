cmake_minimum_required(VERSION 3.21)

project(app)
find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(main main.cpp)
target_link_libraries(main PRIVATE Qt6::Widgets)