cmake_minimum_required(VERSION 3.20)
project(AstroHelperServer)

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
        GIT_REPOSITORY https://github.com/yhirose/cpp-httplib
        GIT_TAG v0.14.0
)
FetchContent_MakeAvailable(httplib)

# ----- cpp-httplib ----- #
FetchContent_Declare(
        fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt
        GIT_TAG 10.1.0)
FetchContent_MakeAvailable(fmt)

# ----- JSON ----- #
FetchContent_Declare(
        json
        GIT_REPOSITORY https://github.com/ArthurSonzogni/nlohmann_json_cmake_fetchcontent
        GIT_TAG v3.11.2)

FetchContent_GetProperties(json)
if(NOT json_POPULATED)
    FetchContent_Populate(json)
    add_subdirectory(${json_SOURCE_DIR} ${json_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

# ----- GATTLib (for bluetooth on Linux) ----- #
if (UNIX AND NOT APPLE)
    SET(GATTLIB_BUILD_DOCS OFF)

    FetchContent_Declare(
            gattlib
            GIT_REPOSITORY https://github.com/labapart/gattlib
            GIT_TAG 33a8a275928b186381bb0aea0f9778e330e57ec3)
    FetchContent_MakeAvailable(gattlib)
endif ()

add_executable(AstroHelperServer src/camera_controller.cpp src/backends/http.cpp src/backends/ble.cpp src/main.cpp)

if (UNIX AND NOT APPLE)
    target_link_libraries(AstroHelperServer PRIVATE ${Gphoto2_LIBRARIES} nlohmann_json::nlohmann_json httplib fmt gattlib)
else()
    target_link_libraries(AstroHelperServer PRIVATE ${Gphoto2_LIBRARIES} nlohmann_json::nlohmann_json httplib fmt)
endif()