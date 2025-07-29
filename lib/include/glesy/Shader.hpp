#pragma once

#include "glesy/Api.h"

namespace glesy {

class Shader {
public:
    Shader(const GLchar* vertexShaderSrc, const GLchar* fragShaderSrc);

    ~Shader();

    [[nodiscard]] GLuint
    id() const;

    void
    use() const;

private:
    GLuint _program{};
};

} // namespace glesy