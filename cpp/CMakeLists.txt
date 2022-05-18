cmake_minimum_required(VERSION 3.5)
project(pvp2048)

set(CMAKE_EXPORT_COMPILE_COMMANDS)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_definitions("-std=c++17")
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()


add_executable(pvp2048 src/gamelogic.cc)
target_link_libraries(pvp2048 ${CONAN_LIBS})
