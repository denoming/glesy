include(GNUInstallDirs)

add_executable(glesy-example02 "")
add_executable(Glesy::Example02 ALIAS glesy-example02)

set_target_properties(glesy-example02
    PROPERTIES
        OUTPUT_NAME example02
)

target_sources(glesy-example02
    PRIVATE
        src/example02.cpp
)

target_link_libraries(glesy-example02 Glesy::Glesy)

target_compile_features(glesy-example02 PRIVATE cxx_std_23)

install(
    TARGETS glesy-example02
    COMPONENT GlesyRuntime
)
