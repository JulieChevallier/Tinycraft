#ifndef LEAVES_HPP
#define LEAVES_HPP

#include "Bloc.hpp"
#include "../Texture/TextureManager.hpp"

class Leaves : public Bloc {
public:
    Leaves(float x, float y, float z) : Bloc(x, y, z, true) {
        setupTexture();
    }

    BlocType getType() const override {
        return BlocType::LEAVES;
    }

protected:
    void setupTexture() override {
        textureID = TextureManager::getInstance().getTexture("src/Ressources/leaves.png");
    }
};

#endif // LEAVES_HPP
