include(GNUInstallDirs)

add_executable(glesy-example06 "")
add_executable(Glesy::Example06 ALIAS glesy-example06)

set_target_properties(glesy-example06
    PROPERTIES
        OUTPUT_NAME example06
)

target_sources(glesy-example06
    PRIVATE
        src/example06.cpp
)

target_link_libraries(glesy-example06 Glesy::Glesy Glad::Glad glfw)

target_compile_features(glesy-example06 PRIVATE cxx_std_23)

install(
    TARGETS glesy-example06
    COMPONENT GlesyRuntime
)
