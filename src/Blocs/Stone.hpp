#ifndef STONE_HPP
#define STONE_HPP

#include "Bloc.hpp"

class Stone : public Bloc {
public:
    Stone(float x, float y, float z) : Bloc(x, y, z, true) {
        setupTexture();
    }

protected:
    void setupTexture() override {
        int width, height, nrChannels;
        unsigned char* data = stbi_load("src/Ressources/stone.png", &width, &height, &nrChannels, 0);
        if (data) {
            glGenTextures(1, &textureID);
            glBindTexture(GL_TEXTURE_2D, textureID);

            GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;

            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            stbi_image_free(data);
        } else {
            std::cerr << "Failed to load texture: " << "src/Ressources/dirt.png" << std::endl;
        }
    }
};

#endif // DIRT_HPP
