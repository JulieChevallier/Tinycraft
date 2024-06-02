#ifndef BLOC_HPP
#define BLOC_HPP

#include <GL/glew.h>
#include <glm/glm.hpp>

class Bloc {
public:
    Bloc(float x, float y, float z);
    virtual ~Bloc();
    void Draw(GLuint shaderProgram) const;
    glm::vec3 getPosition() const { return position; }
    glm::vec3 getMinBounds() const { return position - glm::vec3(0.5f, 0.5f, 0.5f); }
    glm::vec3 getMaxBounds() const { return position + glm::vec3(0.5f, 0.5f, 0.5f); }

private:
    void setColor();

    GLuint VAO, VBO;  
    glm::vec3 color;

protected:
    glm::vec3 position;
};


#endif // BLOC_HPP
