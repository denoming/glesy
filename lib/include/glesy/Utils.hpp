#pragma once

#include "glesy/Api.h"

#include <filesystem>

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
 * Load a shader from file, check for compile errors, print error messages to output log
 * @param type Type of shader (GL_VERTEX_SHADER or GL_FRAGMENT_SHADER)
 * @param path The path to shader source
 * @return A new shader object on success, 0 on failure
 */
GLuint
loadShader(GLenum type, const std::filesystem::path& path);

/**
 * Load a vertex and fragment shader, create a program object, link program.
 * Errors output to log.
 * @param vertexSrc Vertex shader source code
 * @param fragmentSrc Fragment shader source code
 * @return A new program object linked with the vertex/fragment shader pair, 0 on failure
 */
GLuint
loadProgram(const GLchar* vertexSrc, const GLchar* fragmentSrc);

/**
 * Load a vertex and fragment shader, create a program object, link program.
 * Errors output to log.
 * @param vertexPath The path to vertex shader path
 * @param fragmentPath The path to fragment shader path
 * @return A new program object linked with the vertex/fragment shader pair, 0 on failure
 */
GLuint
loadProgram(const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath);

/**
 * Creates program using given vertex and fragment shader ids
 * @param vertexShader The vertex shader id
 * @param fragmentShader The fragment shader id
 * @return The program object id
 */
GLuint
createProgram(GLuint vertexShader, GLuint fragmentShader);

/**
 * Validates given program
 * @warning Slow operation, do not use in drawing routine
 * @param program The program object handle
 * @return @c true if program is valid, @c false - otherwise
 */
[[nodiscard]] bool
validateProgram(GLuint program);

} // namespace glesy
