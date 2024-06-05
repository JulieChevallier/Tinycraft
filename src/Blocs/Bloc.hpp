#ifndef BLOC_HPP
#define BLOC_HPP

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <iostream>

enum class BlocType {
    DIRT,
    GRASS,
    STONE,
    SAND,
    NORMAL
};

class Bloc {
public:
    Bloc(float x, float y, float z, bool useTexture = false);
    virtual ~Bloc();
    virtual void Draw(GLuint shaderProgram) const;
    glm::vec3 getPosition() const { return position; }
    glm::vec3 getMinBounds() const { return position - glm::vec3(0.5f, 0.5f, 0.5f); }
    glm::vec3 getMaxBounds() const { return position + glm::vec3(0.5f, 0.5f, 0.5f); }
    bool getUseTexture() const { return useTexture; }
    virtual BlocType getType() const = 0;

protected:
    virtual void setupTexture() = 0;
    void setColor();

    GLuint VAO, VBO;  
    GLuint textureID;
    glm::vec3 position;
    glm::vec3 color;
    bool useTexture;
};

#endif // BLOC_HPP
