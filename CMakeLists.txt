cmake_minimum_required(VERSION 3.20)
project(chemwar)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE Debug)
set(EXECUTABLE_OUTPUT_PATH ../bin/)
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_mixer)
file(GLOB_RECURSE source src/*.cpp)

link_libraries(SDL2 SDL2_image SDL2_mixer)
add_executable(chemwar ${source})
