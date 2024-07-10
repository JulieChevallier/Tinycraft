#ifndef DIRT_HPP
#define DIRT_HPP

#include "Bloc.hpp"
#include "../Texture/TextureManager.hpp"

class Dirt : public Bloc {
public:
    Dirt(float x, float y, float z) : Bloc(x, y, z, true) {
        setupTexture();
    }

    BlocType getType() const override {
        return BlocType::DIRT;
    }

protected:
    void setupTexture() override {
        textureID = TextureManager::getInstance().getTexture("src/Ressources/dirt.png");
    }
};

#endif // DIRT_HPP