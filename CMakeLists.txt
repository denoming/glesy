cmake_minimum_required(VERSION 3.18)

project(glesy
    VERSION 1.0.0
    DESCRIPTION "Glesy project"
)

include(cmake/ProjectOptions.cmake)
include(cmake/ProjectConfigs.cmake)
include(cmake/ProjectInstall.cmake)

add_subdirectory(dep)
add_subdirectory(lib)
add_subdirectory(src)
