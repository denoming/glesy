set(TARGET glesy)

add_library(${TARGET} STATIC)
add_library(Glesy::Glesy ALIAS ${TARGET})

target_include_directories(${TARGET}
    PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
           $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
    PRIVATE $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
)

target_link_libraries(${TARGET}
    PRIVATE OpenGL::GLES3 OpenGL::EGL  X11::X11
    PUBLIC spdlog::spdlog
)

target_sources(${TARGET}
    PRIVATE
        Platform.cpp
        Utils.cpp
        X11/NativeDisplay.cpp
        X11/NativeWindow.cpp
)

target_compile_features(${TARGET} PUBLIC cxx_std_23)
