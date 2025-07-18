/**
 * Example 01: Demonstrates using GLAD and GLFW libs to create window.
 *             Handling ESC key press and window resize events.
 **/

#include "glesy/Api.h"

#include <spdlog/spdlog.h>
#include <GLFW/glfw3.h>

static constexpr unsigned int kWidth = 800;
static constexpr unsigned int kHeight = 600;

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

    GLFWwindow* window = glfwCreateWindow(kWidth, kHeight, "example01", nullptr, nullptr);
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

    while (not glfwWindowShouldClose(window)) {
        onWindowInput(window);

        // Clear the color buffer
        glClearColor(0.3f, 0.2f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
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
