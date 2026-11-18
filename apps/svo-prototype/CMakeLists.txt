cmake_minimum_required(VERSION 3.20)

project(svo-prototype LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SFML 3 REQUIRED COMPONENTS Window System)
find_package(OpenGL REQUIRED)

add_executable(svo-prototype main.cpp)

target_link_libraries(svo-prototype PRIVATE
    SFML::Window
    SFML::System
    OpenGL::GL
)