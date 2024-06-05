#ifndef GRASS_HPP
#define GRASS_HPP

#include "Bloc.hpp"

class Grass : public Bloc {
public:
    Grass(float x, float y, float z) : Bloc(x, y, z, true) {
        setupTexture();
    }

protected:
    void setupTexture() override;
    void Draw(GLuint shaderProgram) const override;
    GLuint textureSideID;
    GLuint textureTopID;
    GLuint textureBottomID;
};

#endif // GRASS_HPP
