cmake_minimum_required(VERSION 3.20)
project(ProjectGanymedeServer)

include(FetchContent)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/modules/)

# ----- GPhoto2 (more specifically libgphoto2) ----- #
find_package(Gphoto2 REQUIRED COMPONENTS system filesystem)
include_directories(${Gphoto2_INCLUDE_DIRS})
list(APPEND LIBS ${Gphoto2_LIBRARIES})
add_definitions(${Gphoto2_DEFINITIONS})

# ----- Boost ----- #
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

find_package(Boost 1.76 REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

# ----- Boost Beast HTTP Server ----- #
FetchContent_Declare(
        boost_beast
        GIT_REPOSITORY "https://github.com/boostorg/beast"
        GIT_TAG "b7344b0d501f23f763a76488826dde8c31f34b5d" # 1.76
)
FetchContent_MakeAvailable(boost_beast)

# ----- Protobuf ----- #
find_package(Protobuf REQUIRED)

# ----- GATTLib (for bluetooth on Linux) ----- #
if (UNIX AND NOT APPLE)
    FetchContent_Declare(
            gattlib
            GIT_REPOSITORY "https://github.com/labapart/gattlib"
            GIT_TAG "db44d7a99df3e7f71fb71b9431df6201a92d9edf"
    )
    FetchContent_MakeAvailable(gattlib)
endif ()

add_executable(ProjectGanymedeServer src/CameraController.cpp src/main.cpp)

if (UNIX AND NOT APPLE)
    target_link_libraries(ProjectGanymedeServer ${Gphoto2_LIBRARIES} ${Boost_LIBRARIES} gattlib)
else()
    target_link_libraries(ProjectGanymedeServer ${Gphoto2_LIBRARIES} ${Boost_LIBRARIES})
endif()