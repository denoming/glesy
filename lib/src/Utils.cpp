#include "glesy/Utils.hpp"

#include <spdlog/spdlog.h>

#include <vector>
#include <fstream>

namespace glesy {

GLuint
loadShader(const GLenum type, const GLchar* shaderSrc)
{
    const GLuint shader = glCreateShader(type);
    if (shader == 0) {
        SPDLOG_ERROR("Unable to create shader: error<{}>", glGetError());
        return 0;
    }

    glShaderSource(shader, 1, &shaderSrc, nullptr);
    glCompileShader(shader);

    GLint compiled{};
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (compiled != GL_TRUE) {
        GLint infoLen{};
        if (glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen); infoLen > 1) {
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
loadShader(const GLenum type, const std::filesystem::path& path)
{
    try {
        std::ifstream file;
        file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        file.open(path);
        std::stringstream buffer;
        buffer << file.rdbuf();
        file.close();
        return loadShader(type, buffer.view().data());
    } catch (const std::exception& e) {
        SPDLOG_ERROR("Unable to load shader: {}", e.what());
        return 0;
    }
}

GLuint
loadProgram(const GLchar* vertexSrc, const GLchar* fragmentSrc)
{
    const GLuint vertexShader = loadShader(GL_VERTEX_SHADER, vertexSrc);
    if (vertexShader == 0) {
        SPDLOG_ERROR("Unable to load vertex shader");
        return 0;
    }

    const GLuint fragmentShader = loadShader(GL_FRAGMENT_SHADER, fragmentSrc);
    if (fragmentShader == 0) {
        SPDLOG_ERROR("Unable to load fragment shader");
        glDeleteShader(vertexShader);
        return 0;
    }

    const GLuint programId = createProgram(vertexShader, fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return programId;
}

GLuint
loadProgram(const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath)
{
    const GLuint vertexShader = loadShader(GL_VERTEX_SHADER, vertexPath);
    if (vertexShader == 0) {
        SPDLOG_ERROR("Unable to load vertex shader");
        return 0;
    }

    const GLuint fragmentShader = loadShader(GL_FRAGMENT_SHADER, fragmentPath);
    if (fragmentShader == 0) {
        SPDLOG_ERROR("Unable to load fragment shader");
        glDeleteShader(vertexShader);
        return 0;
    }

    const GLuint programId = createProgram(vertexShader, fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return programId;
}

GLuint
createProgram(const GLuint vertexShader, const GLuint fragmentShader)
{
    const GLuint programObject = glCreateProgram();
    if (programObject == 0) {
        SPDLOG_ERROR("Unable to create program: error<{}>", glGetError());
        return 0;
    }

    glAttachShader(programObject, vertexShader);
    glAttachShader(programObject, fragmentShader);

    glLinkProgram(programObject);

    GLint linked{};
    glGetProgramiv(programObject, GL_LINK_STATUS, &linked);
    if (linked != GL_TRUE) {
        GLint infoLen = 0;
        if (glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &infoLen); infoLen > 1) {
            std::vector<GLchar> infoLog(infoLen);
            glGetProgramInfoLog(programObject, infoLen, nullptr, infoLog.data());
            SPDLOG_ERROR("Program linking error: {}", infoLog.data());
        }
        glDeleteProgram(programObject);
        return 0;
    }

    return programObject;
}

[[nodiscard]] bool
validateProgram(const GLuint program)
{
    glValidateProgram(program);

    GLint valid{};
    glGetProgramiv(program, GL_VALIDATE_STATUS, &valid);
    if (valid != GL_TRUE) {
        GLint infoLen = 0;
        if (glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLen); infoLen > 1) {
            std::vector<GLchar> infoLog(infoLen);
            glGetProgramInfoLog(program, infoLen, nullptr, infoLog.data());
            SPDLOG_ERROR("Program validation error: {}", infoLog.data());
        }
        return false;
    }

    return true;
}

} // namespace glesy
