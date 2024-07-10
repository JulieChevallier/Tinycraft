#ifndef STONE_HPP
#define STONE_HPP

#include "Bloc.hpp"
#include "../Texture/TextureManager.hpp"

class Stone : public Bloc {
public:
    Stone(float x, float y, float z) : Bloc(x, y, z, true) {
        setupTexture();
    }

    BlocType getType() const override {
        return BlocType::STONE;
    }

protected:
    void setupTexture() override {
        textureID = TextureManager::getInstance().getTexture("src/Ressources/stone.png");
    }
};

#endif // STONE_HPP