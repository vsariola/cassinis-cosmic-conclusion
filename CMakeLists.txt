cmake_minimum_required(VERSION 3.10)

cmake_policy(SET CMP0091 NEW)

set(CMAKE_VERBOSE_MAKEFILE ON)

project(cassini
    VERSION 0.0.0
    DESCRIPTION "Cassini’s Cosmic Conclusion, a windows 1k intro"
    LANGUAGES C CXX)

# Only do these if this is the main project, and not if it is included through add_subdirectory
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    # Let's ensure -std=c++xx instead of -std=g++xx
    set(CMAKE_CXX_EXTENSIONS OFF)

    # Let's nicely support folders in IDE's
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
endif()

add_subdirectory(src)
