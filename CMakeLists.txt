#cmake_minimum_required(VERSION 3.10)
#project(server)
#file(GLOB SRC ${PROJECT_SOURCE_DIR}/src/*.cpp)
#include_directories(${PROJECT_SOURCE_DIR}/include)
#set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR})
#add_executable(server ${SRC})
cmake_minimum_required(VERSION 3.10)
project(MyServer)

set(CMAKE_CXX_STANDARD 11)

# 添加 include 目录到项目中
include_directories(include)

# 添加所有的源文件
set(SRC
    src/epoll_server.cpp
    src/httpresponse.cpp
    src/server.cpp
    src/main.cpp
    src/log.cpp
)

# 编译可执行文件
add_executable(server ${SRC})
