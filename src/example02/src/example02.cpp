/**
 * Example 02: Demonstrates using VBO and VAO objects.
 *             Use constant vertex attribute to set color.
 **/

#include "glesy/Api.h"
#include "glesy/Shader.hpp"

#include <GLFW/glfw3.h>

#include <spdlog/spdlog.h>

static constexpr unsigned int kWidth{800};
static constexpr unsigned int kHeight{600};
static constexpr int kVertexPosSize{3};
static constexpr int kVertexPosIndex{0};
static constexpr int kVertexColorSize{4};
static constexpr int kVertexColorIndex{1};

// clang-format off
static constexpr GLfloat kVertexData[] = {
    0.0f,  0.5f,  0.0f,       // v0
    1.0f,  0.0f,  0.0f, 1.0f, // c0
    -0.5f, -0.5f, 0.0f,       // v1
    0.0f,  1.0f,  0.0f, 1.0f, // c1
    0.5f,  -0.5f, 0.0f,       // v2
    0.0f,  0.0f,  1.0f, 1.0f, // c2
};
// clang-format on

static auto vShader = R"glsl(
#version 330 core
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
#version 330 core
precision mediump float;
in vec4 v_color;
out vec4 o_fragColor;
void main()
{
    o_fragColor = v_color;
}
)glsl";

static void
onWindowResize(GLFWwindow* window, int width, int height);
static void
onWindowInput(GLFWwindow* window);

int
main()
{
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(kWidth, kHeight, "example02", nullptr, nullptr);
    if (window == nullptr) {
        SPDLOG_ERROR("Failed to create GLFW window");
        glfwTerminate();
        return EXIT_FAILURE;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, onWindowResize);

    if (const int version = gladLoadGL(glfwGetProcAddress); version > 0) {
        SPDLOG_INFO("Initialized OpenGL {}.{} version",
                    GLAD_VERSION_MAJOR(version),
                    GLAD_VERSION_MINOR(version));
    } else {
        SPDLOG_ERROR("Failed to initialize OpenGL context");
        return EXIT_FAILURE;
    }

    {
        const glesy::Shader shader(vShader, fShader);

        glViewport(0, 0, kWidth, kHeight);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

        unsigned int VBO, VAO;
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(kVertexData), kVertexData, GL_STATIC_DRAW);
        glEnableVertexAttribArray(kVertexPosIndex);
        glVertexAttribPointer(kVertexPosIndex,
                              kVertexPosSize,
                              GL_FLOAT,
                              GL_FALSE,
                              sizeof(GLfloat) * (kVertexPosSize + kVertexColorSize),
                              nullptr);
        glEnableVertexAttribArray(kVertexColorIndex);
        glVertexAttribPointer(kVertexColorIndex,
                              kVertexColorSize,
                              GL_FLOAT,
                              GL_FALSE,
                              sizeof(GLfloat) * (kVertexPosSize + kVertexColorSize),
                              reinterpret_cast<const void*>(kVertexPosSize * sizeof(GLfloat)));
        glBindVertexArray(0);

        while (not glfwWindowShouldClose(window)) {
            onWindowInput(window);

            // Clear the color buffer
            glClear(GL_COLOR_BUFFER_BIT);

            // Activate shader program
            shader.use();

            // Bind array arrays object (state) and draw
            glBindVertexArray(VAO);
            glDrawArrays(GL_TRIANGLES, 0, 3);

            // Swap back and front buffers
            glfwSwapBuffers(window);
            glfwPollEvents();
        }
    }

    glfwTerminate();
    return EXIT_SUCCESS;
}

static void
onWindowInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

static void
onWindowResize(GLFWwindow* /*window*/, const int width, const int height)
{
    glViewport(0, 0, width, height);
}
