cmake_minimum_required(VERSION 3.20)

#set(CMAKE_C_COMPILER /usr/bin/x86_64-w64-mingw32-gcc)
#set(CMAKE_CXX_COMPILER /usr/bin/x86_64-w64-mingw32-g++)

project(chemwar)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE Debug)
set(EXECUTABLE_OUTPUT_PATH ../bin/)
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(SDL2_ttf REQUIRED)
file(GLOB_RECURSE source src/*.cpp)

link_libraries(SDL2 SDL2_image SDL2_mixer SDL2_ttf)
add_executable(chemwar ${source}
        src/lib/drefl.h)
