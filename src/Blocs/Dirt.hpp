#ifndef DIRT_HPP
#define DIRT_HPP

#include "Bloc.hpp"
#include <GL/glew.h>
#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> 

class Dirt : public Bloc {
public:
    Dirt(float x, float y, float z); 
    virtual ~Dirt();
    void Draw(GLuint shaderProgram) const override;

private:
    GLuint VAO, VBO, textureID;
    glm::vec3 position; 
};

#endif // DIRT_HPP