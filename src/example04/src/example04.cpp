/**
 * Example 04: Demonstrates using uniforms to set color dynamically
 **/

#include "glesy/Api.h"
#include "glesy/Shader.hpp"

#include <GLFW/glfw3.h>

#include <spdlog/spdlog.h>

static constexpr unsigned int kWidth{800};
static constexpr unsigned int kHeight{600};
static constexpr int kVertexPosSize{3};
static constexpr int kVertexPosIndex{0};

// clang-format off
static constexpr GLfloat kVertexData[] = {
    0.5f,  0.5f, 0.0f,  // top right
    0.5f, -0.5f, 0.0f,  // bottom right
   -0.5f, -0.5f, 0.0f,  // bottom left
   -0.5f,  0.5f, 0.0f,   // top left
};
static constexpr GLuint kIndices[] = {
    0, 1, 3,   // first triangle
    1, 2, 3    // second triangle
};
// clang-format on

static auto vShader = R"glsl(
#version 330 core
layout(location = 0) in vec4 a_position;
void main()
{
    gl_Position = a_position;
}
)glsl";

static auto fShader = R"glsl(
#version 330 core
precision mediump float;
out vec4 o_fragColor;
uniform vec4 ourColor;
void main()
{
    o_fragColor = ourColor;
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

    GLFWwindow* window = glfwCreateWindow(kWidth, kHeight, "example04", nullptr, nullptr);
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
        int vertexColorLocation = glGetUniformLocation(shader.id(), "ourColor");

        glViewport(0, 0, kWidth, kHeight);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
#if 0
        // Enable wareframe mode
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
#else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif

        unsigned int VBO{}, VAO{}, EBO{};
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        {
            glBindVertexArray(VAO);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(kVertexData), kVertexData, GL_STATIC_DRAW);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(kIndices), kIndices, GL_STATIC_DRAW);
            glVertexAttribPointer(kVertexPosIndex,
                                  kVertexPosSize,
                                  GL_FLOAT,
                                  GL_FALSE,
                                  sizeof(GLfloat) * (kVertexPosSize),
                                  (void*)0);
            glEnableVertexAttribArray(kVertexPosIndex);
        }

        while (not glfwWindowShouldClose(window)) {
            onWindowInput(window);

            // Clear the color buffer
            glClear(GL_COLOR_BUFFER_BIT);

            // Activate shader program
            shader.use();

            // Update color uniform value
            const float greenValue = (std::sin(static_cast<float>(glfwGetTime())) / 2.0f) + 0.5f;
            glUniform4f(vertexColorLocation, 0.0f, greenValue, 0.0f, 1.0f);

            // Bind array arrays object (state) and draw
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);

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
