#include "glesy/Utils.hpp"

#include <spdlog/spdlog.h>

#include <vector>

namespace glesy {

GLuint
loadShader(GLenum type, const GLchar* shaderSrc)
{
    // Create the shader object
    const GLuint shader = glCreateShader(type);
    if (shader == 0) {
        return 0;
    }

    // Load the shader source
    glShaderSource(shader, 1, &shaderSrc, nullptr);

    // Compile the shader
    glCompileShader(shader);

    // Check the compile status
    GLint compiled{};
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (not compiled) {
        GLint infoLen{};
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
        if (infoLen > 1) {
            std::vector<GLchar> infoLog(infoLen);
            glGetShaderInfoLog(shader, infoLen, nullptr, infoLog.data());
            SPDLOG_ERROR("Shader compilation error: {}", infoLog.data());
        }
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

GLuint
loadProgram(const GLchar* vertexShaderSrc, const GLchar* fragShaderSrc)
{
    // Load the vertex/fragment shaders
    const GLuint vertexShader = loadShader(GL_VERTEX_SHADER, vertexShaderSrc);
    if (vertexShader == 0) {
        SPDLOG_ERROR("Unable to load vertex shader");
        return 0;
    }

    const GLuint fragmentShader = loadShader(GL_FRAGMENT_SHADER, fragShaderSrc);
    if (fragmentShader == 0) {
        SPDLOG_ERROR("Unable to load fragment shader");
        glDeleteShader(vertexShader);
        return 0;
    }

    // Create the program object
    const GLuint programObject = glCreateProgram();
    if (programObject == 0) {
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return 0;
    }

    glAttachShader(programObject, vertexShader);
    glAttachShader(programObject, fragmentShader);

    // Link the program
    glLinkProgram(programObject);

    // Check the link status
    GLint linked{};
    glGetProgramiv(programObject, GL_LINK_STATUS, &linked);
    if (not linked) {
        GLint infoLen = 0;
        glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &infoLen);
        if (infoLen > 1) {
            std::vector<GLchar> infoLog(infoLen);
            glGetProgramInfoLog(programObject, infoLen, nullptr, infoLog.data());
            SPDLOG_ERROR("Program linking error: {}", infoLog.data());
        }
        glDeleteProgram(programObject);
        return 0;
    }

    // Free up no longer needed shader resources
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return programObject;
}

} // namespace glesy
