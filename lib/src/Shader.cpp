#include "glesy/Shader.hpp"

#include "glesy/Utils.hpp"

#include <stdexcept>

namespace glesy {

Shader::Shader(const GLchar* vertexShaderSrc, const GLchar* fragShaderSrc)
{
    if (_program = loadProgram(vertexShaderSrc, fragShaderSrc); _program == 0) {
        throw std::runtime_error("Failed to load shaders");
    }
}

Shader::~Shader()
{
    glDeleteProgram(_program);
}

GLuint
Shader::id() const
{
    return _program;
}

void
Shader::use() const
{
    glUseProgram(_program);
}

void
Shader::setBool(const std::string& name, const bool value) const
{
    glUniform1i(glGetUniformLocation(_program, name.data()), static_cast<int>(value));
}

void
Shader::setInt(const std::string& name, const int value) const
{
    glUniform1i(glGetUniformLocation(_program, name.data()), value);
}

void
Shader::setFloat(const std::string& name, const float value) const
{
    glUniform1f(glGetUniformLocation(_program, name.data()), value);
}

} // namespace glesy
