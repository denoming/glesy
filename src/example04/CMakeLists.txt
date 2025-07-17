include(GNUInstallDirs)

add_executable(glesy-example04 "")
add_executable(Glesy::Example04 ALIAS glesy-example04)

set_target_properties(glesy-example04
    PROPERTIES
        OUTPUT_NAME example04
)

target_sources(glesy-example04
    PRIVATE
        src/example04.cpp
)

target_link_libraries(glesy-example04 Glesy::Glesy)

target_compile_features(glesy-example04 PRIVATE cxx_std_23)

install(
    TARGETS glesy-example04
    COMPONENT GlesyRuntime
)
