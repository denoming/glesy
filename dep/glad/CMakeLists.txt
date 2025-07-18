cmake_minimum_required(VERSION 3.0)

project(glad C)

add_library(glad SHARED)
add_library(Glad::Glad ALIAS glad)

target_sources(glad
    PRIVATE src/egl.c
            src/gl.c
)

target_include_directories(glad
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
)
