#include "Grass.hpp"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void Grass::setupTexture() {
    int width, height, nrChannels;

    // Charger la texture des côtés
    unsigned char* data = stbi_load("src/Ressources/grass_block_side.png", &width, &height, &nrChannels, 0);
    if (data) {
        glGenTextures(1, &textureSideID);
        glBindTexture(GL_TEXTURE_2D, textureSideID);
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        stbi_image_free(data);
        std::cout << "Texture loaded: grass_block_side.png" << std::endl;
    } else {
        std::cerr << "Failed to load texture: grass_block_side.png" << std::endl;
    }

    // Charger la texture du dessus
    data = stbi_load("src/Ressources/grass_block_top.png", &width, &height, &nrChannels, 0);
    if (data) {
        glGenTextures(1, &textureTopID);
        glBindTexture(GL_TEXTURE_2D, textureTopID);
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        stbi_image_free(data);
        std::cout << "Texture loaded: grass_block_top.png" << std::endl;
    } else {
        std::cerr << "Failed to load texture: grass_block_top.png" << std::endl;
    }

    // Charger la texture du dessous
    data = stbi_load("src/Ressources/dirt.png", &width, &height, &nrChannels, 0);
    if (data) {
        glGenTextures(1, &textureBottomID);
        glBindTexture(GL_TEXTURE_2D, textureBottomID);
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        stbi_image_free(data);
        std::cout << "Texture loaded: dirt.png" << std::endl;
    } else {
        std::cerr << "Failed to load texture: dirt.png" << std::endl;
    }
}

void Grass::Draw(GLuint shaderProgram) const {
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);

    glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    // Dessiner les côtés
    glBindTexture(GL_TEXTURE_2D, textureSideID);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);
    glDrawArrays(GL_TRIANGLES, 0, 24);

    // Dessiner le dessus
    glBindTexture(GL_TEXTURE_2D, textureBottomID);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);
    glDrawArrays(GL_TRIANGLES, 24, 6);

    // Dessiner le dessous
    glBindTexture(GL_TEXTURE_2D, textureTopID);
    glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);
    glDrawArrays(GL_TRIANGLES, 30, 6);

    glBindVertexArray(0);
}
