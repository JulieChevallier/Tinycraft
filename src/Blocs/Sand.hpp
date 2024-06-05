#ifndef SAND_HPP
#define SAND_HPP

#include "Bloc.hpp"
#include "../Texture/TextureManager.hpp"

class Sand : public Bloc {
public:
    Sand(float x, float y, float z) : Bloc(x, y, z, true) {
        setupTexture();
    }

protected:
    void setupTexture() override {
        textureID = TextureManager::getInstance().getTexture("src/Ressources/sand.png");
    }
};

#endif // SAND_HPP
