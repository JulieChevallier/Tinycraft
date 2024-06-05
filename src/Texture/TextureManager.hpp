#ifndef TEXTURE_MANAGER_HPP
#define TEXTURE_MANAGER_HPP

#include <GL/glew.h>
#include <string>
#include <unordered_map>

class TextureManager {
public:
    static TextureManager& getInstance();
    GLuint getTexture(const std::string& filepath);

private:
    TextureManager() = default;
    TextureManager(const TextureManager&) = delete;
    TextureManager& operator=(const TextureManager&) = delete;

    std::unordered_map<std::string, GLuint> textures;
    GLuint loadTexture(const std::string& filepath);
};

#endif // TEXTURE_MANAGER_HPP
