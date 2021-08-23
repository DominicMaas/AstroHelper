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

# ----- cpp-httplib ----- #
FetchContent_Declare(
        httplib
        GIT_REPOSITORY "https://github.com/yhirose/cpp-httplib"
        GIT_TAG "429750092850b18abeccb7bd390e9fb88baf2b1b"
)
FetchContent_MakeAvailable(httplib)

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
    target_link_libraries(ProjectGanymedeServer ${Gphoto2_LIBRARIES} httplib gattlib)
else()
    target_link_libraries(ProjectGanymedeServer ${Gphoto2_LIBRARIES} httplib)
endif()