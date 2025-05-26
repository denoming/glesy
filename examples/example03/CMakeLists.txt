include(GNUInstallDirs)

add_executable(glesy-example03 "")
add_executable(Glesy::Example03 ALIAS glesy-example03)

set_target_properties(glesy-example03
    PROPERTIES
        OUTPUT_NAME example03
)

target_sources(glesy-example03
    PRIVATE
        src/example03.cpp
)

target_link_libraries(glesy-example03 Glesy::Glesy)

target_compile_features(glesy-example03 PRIVATE cxx_std_23)

install(
    TARGETS glesy-example03
    COMPONENT GlesyRuntime
)
