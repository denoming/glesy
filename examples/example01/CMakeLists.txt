include(GNUInstallDirs)

set(TARGET glesy-example01)

add_executable(${TARGET} "")
add_executable(Glesy::Example01 ALIAS ${TARGET})

set_target_properties(${TARGET}
    PROPERTIES
    OUTPUT_NAME example01
)

target_sources(${TARGET}
    PRIVATE
        src/example01.cpp
)

target_link_libraries(${TARGET}
    PRIVATE Glesy::Glesy
)

target_compile_features(${TARGET} PRIVATE cxx_std_20)

install(
    TARGETS ${TARGET}
    COMPONENT GlesyRuntime
)
