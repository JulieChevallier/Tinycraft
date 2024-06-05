#ifndef NORMAL_HPP
#define NORMAL_HPP

#include "Bloc.hpp"

class Normal : public Bloc {
public:
    Normal(float x, float y, float z) : Bloc(x, y, z, false) {
        setColor();
    }

    BlocType getType() const override {
        return BlocType::NORMAL;
    }

protected:
    void setupTexture() override {
        // No texture
    }
};

#endif // COLOREDBLOC_HPP
