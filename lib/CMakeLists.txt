set(TARGET glesy)

add_library(${TARGET} STATIC)
add_library(Glesy::Glesy ALIAS ${TARGET})

target_include_directories(${TARGET}
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
)

target_link_libraries(${TARGET}
    PRIVATE OpenGL::GLES3 OpenGL::EGL  X11::X11
    PUBLIC spdlog::spdlog
)

target_sources(${TARGET}
    PRIVATE
        src/Platform.cpp
        src/Utils.cpp
        src/X11/NativeDisplay.cpp
        src/X11/NativeWindow.cpp
)

target_compile_features(${TARGET} PUBLIC cxx_std_23)
