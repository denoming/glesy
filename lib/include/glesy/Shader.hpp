#pragma once

#include "glesy/Api.h"

#include <string>

namespace glesy {

class Shader {
public:
    Shader(const GLchar* vertexShaderSrc, const GLchar* fragShaderSrc);

    ~Shader();

    [[nodiscard]] GLuint
    id() const;

    void
    use() const;

    void
    setBool(const std::string& name, bool value) const;

    void
    setInt(const std::string& name, int value) const;

    void
    setFloat(const std::string& name, float value) const;

private:
    GLuint _program{};
};

} // namespace glesy