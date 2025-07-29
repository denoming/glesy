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

} // namespace glesy
