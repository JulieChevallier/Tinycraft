#ifndef WOOD_HPP
#define WOOD_HPP

#include "Bloc.hpp"
#include "../Texture/TextureManager.hpp"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Wood : public Bloc {
public:
    Wood(float x, float y, float z) : Bloc(x, y, z, true) {
        setupTexture();
    }

    BlocType getType() const override {
        return BlocType::WOOD;
    }

protected:
    void setupTexture() override {
        textureID = TextureManager::getInstance().getTexture("src/Ressources/log_oak_top.png");
        textureSideID = TextureManager::getInstance().getTexture("src/Ressources/log_oak.png");
    }
void Draw(GLuint shaderProgram) const override {
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);

        glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

        glBindTexture(GL_TEXTURE_2D, textureSideID);
        glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);
        glDrawArrays(GL_TRIANGLES, 0, 24);

        glBindTexture(GL_TEXTURE_2D, textureTopID);
        glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);
        glDrawArrays(GL_TRIANGLES, 24, 6);

        glBindTexture(GL_TEXTURE_2D, textureTopID);
        glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);
        glDrawArrays(GL_TRIANGLES, 30, 6);

        glBindVertexArray(0);
    }
    GLuint textureSideID;
    GLuint textureTopID;
};

#endif // WOOD_HPP
