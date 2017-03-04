cmake_minimum_required(VERSION 2.8.4)
project(csgo_nopp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m64 -std=c++11")
set(EXECUTABLE_OUTPUT_PATH ./build)

set(SOURCE_FILES remote.cpp main.cpp)
add_executable(csgo_nopp ${SOURCE_FILES})

target_link_libraries(csgo_nopp)
