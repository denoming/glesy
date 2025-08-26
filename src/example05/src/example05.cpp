/**
 * Example 05: Demonstrates using textures
 *
 * Warning: Set working directory to project root
 **/

#include "glesy/Api.h"
#include "glesy/Shader.hpp"
#include "glesy/Texture.hpp"

#include <GLFW/glfw3.h>

#include <spdlog/spdlog.h>

static constexpr unsigned int kWidth{800};
static constexpr unsigned int kHeight{600};
static constexpr int kVertexSize{8};
static constexpr int kVertexPosIndex{0};

static float kVertices[] = {
    // positions         // colors           // texture coords
    0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
    0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
   -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
   -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left
};
static unsigned int kIndices[] = {
    0, 1, 3, // first triangle
    1, 2, 3  // second triangle
};

static auto vShader = R"glsl(
#version 330 core
precision mediump float;

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

out vec3 ourColor;
out vec2 texCoord;

void main()
{
    gl_Position = vec4(aPosition, 1.0);
    ourColor = aColor;
    texCoord = vec2(aTexCoord.x, aTexCoord.y);
}
)glsl";

static auto fShader = R"glsl(
#version 330 core
precision mediump float;

out vec4 FragColor;

in vec3 ourColor;
in vec2 texCoord;
uniform sampler2D texture1;

void main()
{
    FragColor = texture(texture1, texCoord) * vec4(ourColor, 1.0);;
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

    GLFWwindow* window = glfwCreateWindow(kWidth, kHeight, "example05", nullptr, nullptr);
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
        shader.use();
        shader.setInt("texture1", 0);

        unsigned int txtId{};
        glGenTextures(1, &txtId);
        glBindTexture(GL_TEXTURE_2D, txtId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        {
            auto texture = Texture::loadPng("assets/container.png");
            glTexImage2D(GL_TEXTURE_2D,
                         0,
                         GL_RGBA,
                         static_cast<GLsizei>(texture.width),
                         static_cast<GLsizei>(texture.height),
                         0,
                         GL_RGBA,
                         GL_UNSIGNED_BYTE,
                         texture.data.data());
        }

        unsigned int VBO{}, VAO{}, EBO{};
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(kVertices), kVertices, GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(kIndices), kIndices, GL_STATIC_DRAW);
        glVertexAttribPointer(0,
                              3,
                              GL_FLOAT,
                              GL_FALSE,
                              8 * sizeof(GLfloat),
                              (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1,
                              3,
                              GL_FLOAT,
                              GL_FALSE,
                              8 * sizeof(GLfloat),
                              (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2,
                              2,
                              GL_FLOAT,
                              GL_FALSE,
                              8 * sizeof(GLfloat),
                              (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);

        while (not glfwWindowShouldClose(window)) {
            onWindowInput(window);

            // Clear the color buffer
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, txtId);

            // Activate shader program
            shader.use();

            // Draw
            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

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