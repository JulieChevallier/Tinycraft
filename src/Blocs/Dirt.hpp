#ifndef DIRT_HPP
#define DIRT_HPP

#include "Bloc.hpp"

class Dirt : public Bloc {
public:
    Dirt();
    virtual ~Dirt();
    void Draw(GLuint shaderProgram) const override;
};

#endif // DIRT_HPP
