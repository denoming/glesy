/**
 * Example 02: Demonstrates using constant vertex attribute and vertex arrays
 **/

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
layout(location = 0) in vec4 a_color;
layout(location = 1) in vec4 a_position;
out vec4 v_color;
void main()
{
    v_color = a_color;
    gl_Position = a_position;
}
)glsl";

static auto fShader = R"glsl(
#version 300 es
precision mediump float;
in vec4 v_color;
out vec4 o_fragColor;
void main()
{
    o_fragColor = v_color;
}
)glsl";

static bool
prepare(void* data)
{
    auto* userData = static_cast<UserData*>(data);
    const GLuint programObject = loadProgram(vShader, fShader);
    if (programObject == 0) {
        return false;
    }
    userData->programObject = programObject;
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
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

    // Set color (set value to a_color with location=0)
    // (each vertices take get the same color)
    static constexpr GLfloat color[] = {1.0f, 0.0f, 0.0f, 1.0f};
    glVertexAttrib4fv(0 /* corresponds to location=0 */, color);

    // Set vertices positions (set value to a_position with location=1)
    // (each vertices get position from this array)
    static constexpr GLfloat vertexPos[3 * 3] = {
        0.0f,  0.5f, 0.0f, // v0
       -0.5f, -0.5f, 0.0f, // v1
        0.5f, -0.5f, 0.0f, // v2
    };
    glVertexAttribPointer(1 /* corresponds to location=1 */, 3, GL_FLOAT, GL_FALSE, 0, vertexPos);

    glEnableVertexAttribArray(1 /* corresponds to location=1 */); // Enable vertex attrib array
    /* Data specified using vertex arrays is copied on each call (not sufficient) */
    glDrawArrays(GL_TRIANGLES, 0, 3 /* specify the number of vertices */);
    glDisableVertexAttribArray(1 /* corresponds to location=1 */); // Disable vertex attrib array
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
