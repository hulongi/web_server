cmake_minimum_required(VERSION 3.10)
project(ser01)
file(GLOB SRC ${PROJECT_SOURCE_DIR}/src/*.cpp)
include_directories(${PROJECT_SOURCE_DIR}/include)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR})
link_directories(/usr/local/lib)
link_libraries(libevent.a)
add_executable(server ${SRC})