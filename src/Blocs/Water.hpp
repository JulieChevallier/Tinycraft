#ifndef WATER_HPP
#define WATER_HPP

#include "Bloc.hpp"
#include "../Texture/TextureManager.hpp"

class Water : public Bloc {
public:
    Water(float x, float y, float z) : Bloc(x, y, z, true) {
        setupTexture();
    }

    BlocType getType() const override {
        return BlocType::WATER;
    }

protected:
    void setupTexture() override {
        textureID = TextureManager::getInstance().getTexture("src/Ressources/water.png");
    }
};

#endif // WATER_HPP
