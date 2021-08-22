cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

#include(FetchContent)

#FetchContent_Declare(
#        extern_gattlib#

       # GIT_REPOSITORY https://github.com/labapart/gattlib
       # GIT_TAG        dev)

#FetchContent_GetProperties(extern_gattlib)
#if(NOT extern_gattlib_POPULATED)
#    FetchContent_Populate(extern_gattlib)
#    add_subdirectory(
#            ${extern_gattlib_SOURCE_DIR}
#            ${extern_gattlib_BINARY_DIR}
#            EXCLUDE_FROM_ALL)
#endif()