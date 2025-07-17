/**
 * Example 04: Demonstrates using vertex buffer object (VBO) and vertx array object (VAO)
 * Using VAO allows to save VBOs state and activate before start drawing
 **/

#include <iostream>

#include "glesy/X11/NativeWindow.hpp"
#include "glesy/X11/NativeDisplay.hpp"
#include "glesy/Api.h"
#include "glesy/Platform.hpp"
#include "glesy/Utils.hpp"

#include <spdlog/spdlog.h>

using namespace glesy;

#define VERTEX_POS_SIZE 3   // x, y, and z
#define VERTEX_COLOR_SIZE 4 // r, g, b, and a
#define VERTEX_POS_INDEX 0
#define VERTEX_COLOR_INDEX 1

static constexpr unsigned int kWidth = 800;
static constexpr unsigned int kHeight = 600;

struct UserData {
    GLuint programObject{};
    GLuint vboIds[2]{};
    GLuint vaoId{};
};

static auto vShader = R"glsl(
#version 300 es
layout(location = 0) in vec4 a_position;
layout(location = 1) in vec4 a_color;
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
    const GLuint programObject = loadProgram(vShader, fShader);
    if (programObject == 0) {
        return false;
    }

    auto* userData = static_cast<UserData*>(data);
    userData->programObject = programObject;

    { /* Creates VBOs and load vertex array data */
        // The vertices, with (x, y, z) and (r, g, b, a) per-vertex
        constexpr GLfloat vertices[3 * (VERTEX_POS_SIZE + VERTEX_COLOR_SIZE)] = {
            0.0f,  0.5f,  0.0f,       // v0
            1.0f,  0.0f,  0.0f, 1.0f, // c0
            -0.5f, -0.5f, 0.0f,       // v1
            0.0f,  1.0f,  0.0f, 1.0f, // c1
            0.5f,  -0.5f, 0.0f,       // v2
            0.0f,  0.0f,  1.0f, 1.0f, // c2
        };

        // Index buffer data
        constexpr GLushort indices[3] = {0, 1, 2};

        // Create vertex array objects
        glGenBuffers(2, userData->vboIds);
        // Load data into 1st VBO
        glBindBuffer(GL_ARRAY_BUFFER, userData->vboIds[0]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // Load data into 2nd VBO
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, userData->vboIds[1]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    }

    { /* Creates VAO with a full state vector */
        // Generate VAO ID
        glGenVertexArrays(1, &userData->vaoId);

        // Bind the VAO and then set up the vertex attributes
        glBindVertexArray(userData->vaoId);

        glBindBuffer(GL_ARRAY_BUFFER, userData->vboIds[0]);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, userData->vboIds[1]);

        glEnableVertexAttribArray(VERTEX_POS_INDEX);
        glEnableVertexAttribArray(VERTEX_COLOR_INDEX);

        glVertexAttribPointer(VERTEX_POS_INDEX,
                              VERTEX_POS_SIZE,
                              GL_FLOAT,
                              GL_FALSE,
                              sizeof(GLfloat) * (VERTEX_POS_SIZE + VERTEX_COLOR_SIZE),
                              nullptr /* no offset */);
        glVertexAttribPointer(VERTEX_COLOR_INDEX,
                              VERTEX_COLOR_SIZE,
                              GL_FLOAT,
                              GL_FALSE,
                              sizeof(GLfloat) * (VERTEX_POS_SIZE + VERTEX_COLOR_SIZE),
                              reinterpret_cast<const void*>(VERTEX_POS_SIZE * sizeof(GLfloat)));

        // Reset to the default VAO
        glBindVertexArray(0);
    }

    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

    return true;
}

static void
draw(void* data)
{
    const auto* userData = static_cast<UserData*>(data);

    glViewport(0, 0, kWidth, kHeight);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(userData->programObject);

    // Draw using the VAO settings
    glBindVertexArray(userData->vaoId);
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, 0);
    glBindVertexArray(0);
}

static void
shutdown(void* data)
{
    const auto* userData = static_cast<UserData*>(data);

    glDeleteProgram(userData->programObject);
    glDeleteBuffers(2, userData->vboIds);
    glDeleteVertexArrays(1, &userData->vaoId);
}

int
main()
{
    x11::NativeDisplay display;
    x11::NativeWindow window{display};
    window.create("Example03", kWidth, kHeight);

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
