#ifndef SHADER_HPP
#define SHADER_HPP

#include <GL/glew.h>
#include <string>

class Shader {
public:
    // Lit le contenu d'un fichier shader et renvoie le code source sous forme de chaîne de caractères
    static std::string readShaderSource(const std::string& fileName);

    // Compile un shader à partir d'une source donnée et renvoie l'ID du shader compilé
    static GLuint compileShader(const std::string& source, GLenum type);

    // Crée un programme de shader à partir des sources de shaders vertex et fragment, les compile, les lie et renvoie l'ID du programme
    static GLuint createShaderProgram(const std::string& vertexShaderSource, const std::string& fragmentShaderSource);
};

#endif // SHADER_HPP
