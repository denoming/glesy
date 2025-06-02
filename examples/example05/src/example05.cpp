/**
 * Example 03: Demonstrates using vertex buffer object (VBO)
 * Using VBO much more efficient than loading data from client memory into vertex array directly
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

void
drawPrimitiveWithMapping(UserData& data,
                         const GLint numVertices,
                         const GLfloat* vtxBuf,
                         const GLint vtxStride,
                         const GLint numIndices,
                         const GLushort* indices)
{
    if (data.vboIds[0] == 0 and data.vboIds[1] == 0) {
        // Create vertex array objects
        glGenBuffers(2, data.vboIds);

        glBindBuffer(GL_ARRAY_BUFFER, data.vboIds[0]);
        glBufferData(GL_ARRAY_BUFFER,
                     vtxStride * numVertices,
                     nullptr /* Do not copy data, only initialize */,
                     GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data.vboIds[1]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     sizeof(GLushort) * numIndices,
                     nullptr /* Do not copy data, only initialize */,
                     GL_STATIC_DRAW);

        {
            // Create VBO #1 storage mapping
            auto* mappedBuf = static_cast<GLfloat*>(
                glMapBufferRange(GL_ARRAY_BUFFER,
                                 0,
                                 vtxStride * numVertices,
                                 GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT));
            if (mappedBuf == nullptr) {
                SPDLOG_ERROR("Unable to map vertex buffer");
                return;
            }
            // Copy the data into the mapped buffer
            memcpy(mappedBuf, vtxBuf, vtxStride * numVertices);
            // Unmap the buffer
            if (glUnmapBuffer(GL_ARRAY_BUFFER) == GL_FALSE) {
                SPDLOG_ERROR("Error unmapping element array buffer object.");
                return;
            }
        }

        {
            // Create VBO #2 storage mapping
            auto* mappedBuf = static_cast<GLushort*>(
                glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER,
                                 0,
                                 sizeof(GLushort) * numIndices,
                                 GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT));
            if (mappedBuf == nullptr) {
                SPDLOG_ERROR("Unable to map vertex buffer");
                return;
            }
            // Copy the data into the mapped buffer
            memcpy(mappedBuf, indices, sizeof(GLushort) * numIndices);
            // Unmap the buffer
            if (glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER) == GL_FALSE) {
                SPDLOG_ERROR("Error unmapping element array buffer object.");
                return;
            }
        }
    }

    glBindBuffer(GL_ARRAY_BUFFER, data.vboIds[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data.vboIds[1]);

    glEnableVertexAttribArray(VERTEX_POS_INDEX);
    glEnableVertexAttribArray(VERTEX_COLOR_INDEX);

    GLuint offset = 0;
    glVertexAttribPointer(VERTEX_POS_INDEX,
                          VERTEX_POS_SIZE,
                          GL_FLOAT,
                          GL_FALSE,
                          vtxStride,
                          reinterpret_cast<const void*>(offset));

    offset += VERTEX_POS_SIZE * sizeof(GLfloat);
    glVertexAttribPointer(VERTEX_COLOR_INDEX,
                          VERTEX_COLOR_SIZE,
                          GL_FLOAT,
                          GL_FALSE,
                          vtxStride,
                          reinterpret_cast<const void*>(offset));

    glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT, 0);

    glDisableVertexAttribArray(VERTEX_POS_INDEX);
    glDisableVertexAttribArray(VERTEX_COLOR_INDEX);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

static bool
prepare(void* data)
{
    const GLuint programObject = loadProgram(vShader, fShader);
    if (programObject == 0) {
        return false;
    }

    auto* userData = static_cast<UserData*>(data);
    userData->programObject = programObject;
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    return true;
}

static void
draw(void* data)
{
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

    auto* userData = static_cast<UserData*>(data);
    glViewport(0, 0, kWidth, kHeight);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(userData->programObject);

    drawPrimitiveWithMapping(*userData,
                             3,
                             vertices,
                             sizeof(GLfloat) * (VERTEX_POS_SIZE + VERTEX_COLOR_SIZE),
                             3,
                             indices);
}

static void
shutdown(void* data)
{
    const auto* userData = static_cast<UserData*>(data);

    glDeleteBuffers(2, userData->vboIds);
    glDeleteProgram(userData->programObject);
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
