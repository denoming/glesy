include(GNUInstallDirs)

add_executable(glesy-example05 "")
add_executable(Glesy::Example05 ALIAS glesy-example05)

set_target_properties(glesy-example05
    PROPERTIES
        OUTPUT_NAME example05
)

target_sources(glesy-example05
    PRIVATE
        src/example05.cpp
)

target_link_libraries(glesy-example05 Glesy::Glesy PNG::PNG)

target_compile_features(glesy-example05 PRIVATE cxx_std_23)

install(
    TARGETS glesy-example05
    COMPONENT GlesyRuntime
)
