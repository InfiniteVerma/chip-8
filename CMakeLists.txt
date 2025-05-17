cmake_minimum_required(VERSION 3.16)
project(Chip8Emulator)

set(CMAKE_BUILD_TYPE Debug)
set(DENABLE_SANITIZERS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_EXPORT_COMPILE_COMMANDS on)

add_subdirectory(src)
add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)
include_directories(include)
