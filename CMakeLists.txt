cmake_minimum_required(VERSION 3.20)
project(ProjectGanymedeServer)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/modules/)

add_subdirectory(extern)

add_executable(ProjectGanymedeServer src/main.cpp)

target_include_directories(ProjectGanymedeServer PRIVATE extern/headers)
target_link_libraries(ProjectGanymedeServer ${Gphoto2_LIBRARIES})
