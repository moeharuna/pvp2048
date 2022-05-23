cmake_minimum_required(VERSION 3.5)
project(pvp2048)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_definitions("-std=c++17")
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

set(GRPC_OUT
  src/proto_out/game.grpc.pb.cc
  )
add_library(grpc ${GRPC_OUT})
set(SERVER_FILES
  src/gamelogic.cc
  )

add_executable(pvp2048 ${SERVER_FILES})
target_link_libraries(pvp2048 ${CONAN_LIBS})
target_link_libraries(pvp2048 ${grpc})
