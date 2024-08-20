#pragma once

#include "glesy/Api.h"

namespace glesy {

/**
 * Load a shader, check for compile errors, print error messages to output log
 * @param type Type of shader (GL_VERTEX_SHADER or GL_FRAGMENT_SHADER)
 * @param shaderSrc Shader source string
 * @return A new shader object on success, 0 on failure
 */
GLuint
loadShader(GLenum type, const GLchar* shaderSrc);

/**
 * Load a vertex and fragment shader, create a program object, link program.
 * Errors output to log.
 * @param vertexShaderSrc Vertex shader source code
 * @param fragShaderSrc Fragment shader source code
 * @return A new program object linked with the vertex/fragment shader pair, 0 on failure
 */
GLuint
loadProgram(const GLchar* vertexShaderSrc, const GLchar* fragShaderSrc);

/**
 * Validates given program
 * @warning Slow operation, do not use in drawing routine
 * @param program The program object handle
 * @return @c true if program is valid, @c false - otherwise
 */
[[nodiscard]] bool
validateProgram(GLuint program);

} // namespace glesy
