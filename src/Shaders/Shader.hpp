#ifndef SHADER_HPP
#define SHADER_HPP

#include <GL/glew.h>
#include <string>

class Shader {
public:
    static std::string readShaderSource(const std::string& fileName);
    static GLuint compileShader(const std::string& source, GLenum type);
    static GLuint createShaderProgram(const std::string& vertexShaderSource, const std::string& fragmentShaderSource);
};

#endif // SHADER_HPP
