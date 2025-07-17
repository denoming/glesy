#include <iostream>

#include "glesy/X11/NativeWindow.hpp"
#include "glesy/X11/NativeDisplay.hpp"
#include "glesy/Api.h"
#include "glesy/Platform.hpp"
#include "glesy/Utils.hpp"

#include <spdlog/spdlog.h>

using namespace glesy;

static constexpr unsigned int kWidth = 800;
static constexpr unsigned int kHeight = 600;

struct UserData {
    GLuint programObject{};
};

static auto vShader = R"glsl(
#version 300 es
layout(location = 0) in vec4 vPosition;
void main()
{
    gl_Position = vPosition;
}
)glsl";

static auto fShader = R"glsl(
#version 300 es
precision mediump float;
out vec4 fragColor;
void main()
{
    fragColor = vec4 ( 1.0, 0.0, 0.0, 1.0 );
}
)glsl";

static bool
prepare(void* data)
{
    auto* userData = static_cast<UserData*>(data);
    userData->programObject = loadProgram(vShader, fShader);
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    return true;
}

static void
draw(void* data)
{
    const auto* userData = static_cast<UserData*>(data);

    // Set the viewport
    glViewport(0, 0, kWidth, kHeight);

    // Clear the color buffer
    glClear(GL_COLOR_BUFFER_BIT);

    // Use the program object
    glUseProgram(userData->programObject);

    // Load the vertex data
    GLfloat vVertices[] = {0.0f, 0.5f, 0.0f, -0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f};
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, vVertices);
    glEnableVertexAttribArray(0);

    glDrawArrays(GL_TRIANGLES, 0, 3);
}

static void
shutdown(void* data)
{
    const auto* userData = static_cast<UserData*>(data);

    glDeleteProgram(userData->programObject);
}

int
main()
{
    x11::NativeDisplay display;
    x11::NativeWindow window{display};
    window.create("Example01", kWidth, kHeight);

    Platform platform;
    if (not platform.configure(display, window, ES_WINDOW_RGB)) {
        return EXIT_FAILURE;
    }

    platform.registerPrepareFunc(std::bind_front(&prepare));
    platform.registerDrawFunc(std::bind_front(&draw));
    platform.registerShutdownFunc(std::bind_front(&shutdown));

    UserData data;
    if (not platform.process(window, &data)) {
        std::cerr << "Error processing" << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
