cmake_minimum_required(VERSION 3.20)

# ----- GPhoto2 (more specifically libgphoto2) ----- #
find_package(Gphoto2 REQUIRED)
include_directories(${Gphoto2_INCLUDE_DIRS})
list(APPEND LIBS ${Gphoto2_LIBRARIES})
add_definitions(${Gphoto2_DEFINITIONS})