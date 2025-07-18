#include <spdlog/spdlog.h>

// GLAD
#include <glad/gl.h>
// GLFW
#include <GLFW/glfw3.h>

void
onWindowResize(GLFWwindow* window, int width, int height);
void
onWindowInput(GLFWwindow* window);

constexpr unsigned int kWidth = 800;
constexpr unsigned int kHeight = 600;

int
main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(kWidth, kHeight, "example06", nullptr, nullptr);
    if (window == nullptr) {
        SPDLOG_ERROR("Failed to create GLFW window");
        glfwTerminate();
        return EXIT_FAILURE;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, onWindowResize);

    // Load OpenGL functions, gladLoadGL returns the loaded version, 0 on error.
    const int version = gladLoadGL(glfwGetProcAddress);
    if (version == 0) {
        SPDLOG_ERROR("Failed to initialize OpenGL context");
        return EXIT_FAILURE;
    }
    SPDLOG_INFO("Initialized OpenGL {}.{} version",
                GLAD_VERSION_MAJOR(version),
                GLAD_VERSION_MINOR(version));

    // render loop
    // -----------
    while (not glfwWindowShouldClose(window)) {
        // input
        // -----
        onWindowInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return EXIT_SUCCESS;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react
// accordingly
// ---------------------------------------------------------------------------------------------------------
void
onWindowInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void
onWindowResize(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}