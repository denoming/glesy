include(GNUInstallDirs)

add_executable(glesy-example01 "")
add_executable(Glesy::Example01 ALIAS glesy-example01)

set_target_properties(glesy-example01
    PROPERTIES
        OUTPUT_NAME example01
)

target_sources(glesy-example01
    PRIVATE
        src/example01.cpp
)

target_link_libraries(glesy-example01 Glesy::Glesy)

target_compile_features(glesy-example01 PRIVATE cxx_std_23)

install(
    TARGETS glesy-example01
    COMPONENT GlesyRuntime
)
