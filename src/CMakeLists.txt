include(GNUInstallDirs)
include(GenerateExportHeader)
include(CMakePackageConfigHelpers)

set(TARGET glesy)

add_library(${TARGET} SHARED)
add_library(Glesy::Glesy ALIAS ${TARGET})

set_target_properties(${TARGET}
    PROPERTIES
        OUTPUT_NAME glesy
        EXPORT_NAME glesy
)

set_target_properties(${TARGET}
    PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
        CXX_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN YES
        COMPATIBLE_INTERFACE_BOOL FLOAT_SUPPORT
        INTERFACE_FLOAT_SUPPORT YES
)

target_include_directories(${TARGET}
    PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
           $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
)

target_sources(${TARGET}
    PRIVATE
        Context.cpp
        ContextImpl.cpp
)

target_compile_features(${TARGET} PUBLIC cxx_std_20)

# Generate export header file with export macros
generate_export_header(${TARGET}
    BASE_NAME GLESY
    EXPORT_FILE_NAME GlesyExport.hpp
)

# Install target files except namelink file
install(TARGETS ${TARGET}
    EXPORT GlesyRuntime
    LIBRARY
        DESTINATION ${CMAKE_INSTALL_LIBDIR}
        COMPONENT GlesyRuntime
        NAMELINK_SKIP
    INCLUDES
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Install target namelink file
install(TARGETS ${TARGET}
    EXPORT GlesyDevelop
    LIBRARY
        DESTINATION ${CMAKE_INSTALL_LIBDIR}
        COMPONENT GlesyDevelop
        NAMELINK_ONLY
    ARCHIVE
        DESTINATION ${CMAKE_INSTALL_LIBDIR}
        COMPONENT GlesyDevelop
    INCLUDES
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Install package config file
install(EXPORT GlesyRuntime
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/glesy
    NAMESPACE Glesy::
    COMPONENT GlesyRuntime
    FILE GlesyRuntime.cmake
)

# Install package config file
install(EXPORT GlesyDevelop
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/glesy
    NAMESPACE Glesy::
    COMPONENT GlesyDevelop
    FILE GlesyDevelop.cmake
)

# Install library header files
install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/
    TYPE INCLUDE
    COMPONENT GlesyDevelop
    FILES_MATCHING
        PATTERN *.hpp
)

# Install export header file
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/GlesyExport.hpp
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/glesy
    COMPONENT GlesyDevelop
)

# Generate package config and version file
configure_package_config_file(
    ${PROJECT_SOURCE_DIR}/config/GlesyConfig.cmake.in
    GlesyConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/glesy
    NO_SET_AND_CHECK_MACRO
    NO_CHECK_REQUIRED_COMPONENTS_MACRO
)
write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/GlesyConfigVersion.cmake
    COMPATIBILITY SameMajorVersion
)

# Install package config and version file
install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/GlesyConfig.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/glesy
    COMPONENT GlesyDevelop
)
install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/GlesyConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/glesy
    COMPONENT GlesyDevelop
)
