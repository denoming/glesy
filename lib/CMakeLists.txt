set(TARGET glesy)

add_library(${TARGET} STATIC)
add_library(Glesy::Glesy ALIAS ${TARGET})

target_include_directories(${TARGET}
    PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
)

target_link_libraries(${TARGET}
    PUBLIC Glad::Glad
           glfw
    PUBLIC spdlog::spdlog
)

target_sources(${TARGET}
    PRIVATE
        src/Utils.cpp
        src/Shader.cpp
        src/Texture.cpp
)

target_compile_features(${TARGET} PUBLIC cxx_std_23)
