cmake_minimum_required(VERSION 3.20)

project(svo-prototype LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SDL3 CONFIG REQUIRED)
find_package(Vulkan REQUIRED)

add_executable(svo-prototype main.cpp)

target_link_libraries(svo-prototype PRIVATE
    SDL3::SDL3
    Vulkan::Vulkan
)