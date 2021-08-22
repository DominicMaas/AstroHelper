cmake_minimum_required(VERSION 3.20)
project(ProjectGanymedeServer)

set(CMAKE_CXX_STANDARD 23)

add_subdirectory(extern)

add_executable(ProjectGanymedeServer src/main.cpp)
