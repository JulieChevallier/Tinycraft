#ifndef BLOC_HPP
#define BLOC_HPP

#include <GL/glew.h>

class Bloc {
protected:

public:
    Bloc();
    virtual ~Bloc();

    virtual void Draw(GLuint shaderProgram) const = 0;
};

#endif // BLOC_HPP
