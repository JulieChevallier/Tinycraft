#ifndef BLOC_HPP
#define BLOC_HPP

#include <GL/glew.h>

class Bloc {
protected:
    static float vertices[108]; 
    static GLuint VAO; 
    static GLuint VBO; 
    static bool initialized; 

public:
    Bloc();
    virtual ~Bloc();

    virtual void Draw(GLuint shaderProgram) const = 0;
    static void InitializeGeometry();
    static void CleanupGeometry();
};

#endif // BLOC_HPP
