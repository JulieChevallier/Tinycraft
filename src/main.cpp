#include <GL/glew.h>
#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <tuple>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <unordered_map>
#include <memory>
#include "Shaders/Shader.hpp"
#include "Blocs/Dirt.cpp"
#include "Blocs/Grass.cpp"
#include "Blocs/Stone.cpp"
#include "PerlinNoise/PerlinNoise.hpp"

using BlocPtr = std::unique_ptr<Bloc>;
using Chunk = std::vector<BlocPtr>;
using Coord = std::tuple<int, int, int>;
struct CoordHash {
    std::size_t operator()(const std::tuple<int, int, int>& coord) const noexcept {
        auto [x, y, z] = coord;
        std::size_t h1 = std::hash<int>{}(x);
        std::size_t h2 = std::hash<int>{}(y);
        std::size_t h3 = std::hash<int>{}(z);
        return h1 ^ (h2 << 1) ^ (h3 << 2);
    }
};

const int CHUNK_SIZE = 16;
const int NUM_CHUNKS_PER_SIDE = 3;

std::unordered_map<Coord, Chunk, CoordHash> chunkMap;
std::unordered_map<Coord, Bloc*, CoordHash> blocMap;
std::vector<Coord> chunkCoordAlreadyGenerated;
std::vector<Coord> listeBlocAVerif;

void addBlocToMap(Bloc* bloc) {
    int x = static_cast<int>(std::round(bloc->getPosition().x));
    int y = static_cast<int>(std::round(bloc->getPosition().y));
    int z = static_cast<int>(std::round(bloc->getPosition().z));
    Coord key(x, y, z);
    blocMap[key] = bloc;
}

// Use for smoothChunk when I will have time to implement it
// int findBlocAtXZ(int x, int z) {
//     for (const auto& entry : blocMap) {
//         int bx = std::get<0>(entry.first);
//         int bz = std::get<2>(entry.first);
//         if (bx == x && bz == z) {
//             return std::get<1>(entry.first);
//         }
//     }
//     return -1;
// }

// Smoothing the terrain by adding blocs between the blocs
// TODO: Make an algorithm which is more useful
void smoothChunk(std::vector<Coord>& listeBlocAVerif) {
    std::vector<Coord> toRemove;

    for (const auto& entry : listeBlocAVerif) {
        int x = std::get<0>(entry);
        int y = std::get<1>(entry);
        int z = std::get<2>(entry);
        for (int i = y - 3; i < y; ++i) {
            BlocPtr bloc = std::make_unique<Bloc>(static_cast<float>(x), static_cast<float>(i), static_cast<float>(z));
            addBlocToMap(bloc.get());
            chunkMap[Coord(x, i, z)].emplace_back(std::move(bloc));
        }
        toRemove.push_back(entry);
    }

    for (const auto& rem : toRemove) {
        listeBlocAVerif.erase(std::remove(listeBlocAVerif.begin(), listeBlocAVerif.end(), rem), listeBlocAVerif.end());
    }
}

// Generate a chunk of blocs
Chunk generateChunk(PerlinNoise& perlin, Coord startCoord, Coord endCoord, double scale, double heightMultiplier, double heightOffset) {
    Chunk blocs;
    for (int x = 0; x < CHUNK_SIZE + 16; ++x) {
        for (int z = 0; z < CHUNK_SIZE; ++z) {
            int coordX = std::get<0>(startCoord) + x - 16;
            int coordZ = std::get<2>(startCoord) + z;
            double noise = perlin.noise(coordX * scale, 0.0, coordZ * scale);
            int height = static_cast<int>(noise * heightMultiplier + heightOffset);
            BlocPtr bloc = std::make_unique<Bloc>(static_cast<float>(coordX), static_cast<float>(height), static_cast<float>(coordZ));
            addBlocToMap(bloc.get());
            blocs.emplace_back(std::move(bloc));
            listeBlocAVerif.push_back(Coord(coordX, height, coordZ));
            chunkMap[Coord(coordX, height, coordZ)] = std::move(blocs);
        }
    }
    return blocs;
}

// Update the chunks around the camera when the camera moves
void updateChunksAroundCamera(PerlinNoise& perlin, Coord cameraChunkCoord, double scale, double heightMultiplier, double heightOffset) {
    int chunkRadius = 1;
    chunkCoordAlreadyGenerated.push_back(cameraChunkCoord);
    for (int i = -chunkRadius; i <= chunkRadius; ++i) {
        for (int j = -chunkRadius; j <= chunkRadius; ++j) {
            Coord neighborChunkCoord(std::get<0>(cameraChunkCoord) + i * CHUNK_SIZE, 0, std::get<2>(cameraChunkCoord) + j * CHUNK_SIZE);
            if (blocMap.find(neighborChunkCoord) == blocMap.end()) {
                Coord endCoord = {std::get<0>(neighborChunkCoord) + CHUNK_SIZE - 1, 0, std::get<2>(neighborChunkCoord) + CHUNK_SIZE - 1};
                Chunk newChunk = generateChunk(perlin, neighborChunkCoord, endCoord, scale, heightMultiplier, heightOffset);
                smoothChunk(listeBlocAVerif);
                chunkMap.emplace(neighborChunkCoord, std::move(newChunk));
            }
        }
    }
}

// Frustum culling to avoid rendering blocs that are not in the camera's view
struct Plane {
    glm::vec3 normal;
    float distance;

    Plane() : normal(glm::vec3(0.0f, 0.0f, 0.0f)), distance(0.0f) {}

    Plane(const glm::vec3& n, float d) : normal(n), distance(d) {}

    void fromPoints(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c) {
        normal = glm::normalize(glm::cross(b - a, c - a));
        distance = -glm::dot(normal, a);
    }

    float distanceToPoint(const glm::vec3& point) const {
        return glm::dot(normal, point) + distance;
    }
};

// Frustum culling to avoid rendering blocs that are not in the camera's view
struct Frustum {
    Plane planes[6];

    void update(const glm::mat4& viewProjection) {
        const float margin = 2.5f;
        planes[0] = Plane(glm::vec3(viewProjection[0].w + viewProjection[0].x, 
                                    viewProjection[1].w + viewProjection[1].x, 
                                    viewProjection[2].w + viewProjection[2].x), 
                        viewProjection[3].w + viewProjection[3].x + margin); // Plan left

        planes[1] = Plane(glm::vec3(viewProjection[0].w - viewProjection[0].x, 
                                    viewProjection[1].w - viewProjection[1].x, 
                                    viewProjection[2].w - viewProjection[2].x), 
                        viewProjection[3].w - viewProjection[3].x + margin); // Plan right

        planes[2] = Plane(glm::vec3(viewProjection[0].w + viewProjection[0].y, 
                                    viewProjection[1].w + viewProjection[1].y, 
                                    viewProjection[2].w + viewProjection[2].y), 
                        viewProjection[3].w + viewProjection[3].y + margin); // Plan bottom

        planes[3] = Plane(glm::vec3(viewProjection[0].w - viewProjection[0].y, 
                                    viewProjection[1].w - viewProjection[1].y, 
                                    viewProjection[2].w - viewProjection[2].y), 
                        viewProjection[3].w - viewProjection[3].y + margin); // Plan top

        planes[4] = Plane(glm::vec3(viewProjection[0].w + viewProjection[0].z, 
                                    viewProjection[1].w + viewProjection[1].z, 
                                    viewProjection[2].w + viewProjection[2].z), 
                        viewProjection[3].w + viewProjection[3].z + margin); // Plan near

        planes[5] = Plane(glm::vec3(viewProjection[0].w - viewProjection[0].z, 
                                    viewProjection[1].w - viewProjection[1].z, 
                                    viewProjection[2].w - viewProjection[2].z), 
                        viewProjection[3].w - viewProjection[3].z + margin); // Plan far

        // Normalize the planes
        for (int i = 0; i < 6; ++i) {
            float length = glm::length(planes[i].normal);
            planes[i].normal = -planes[i].normal;
            planes[i].distance = -planes[i].distance;
        }
    }

    bool intersects(const glm::vec3& min, const glm::vec3& max) const {
        const float tolerance = 0.5f;

        for (int i = 0; i < 6; ++i) {
            const Plane& plane = planes[i];

            // Chooses the extreme point of the bloc
            glm::vec3 pVertex = min;
            if (plane.normal.x > 0) pVertex.x = max.x;
            if (plane.normal.y > 0) pVertex.y = max.y;
            if (plane.normal.z > 0) pVertex.z = max.z;

            // If the bloc is outside the frustum
            if (plane.distanceToPoint(pVertex) - tolerance > 0) {
                return false;
            }
        }
        return true;
    }
};


int main() {
    sf::Font font;
    if (!font.loadFromFile("src/Ressources/Minecraft.ttf")) {
        std::cerr << "Failed to load font!" << std::endl;
        return -1;
    }

    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(24); 
    text.setFillColor(sf::Color::White);
    text.setPosition(10.f, 10.f); 

    sf::RenderWindow window(sf::VideoMode(1600, 1200), "TinyCraft", sf::Style::Default, sf::ContextSettings(24));
    window.setFramerateLimit(60);
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    std::string vertexSource = Shader::readShaderSource("src/Shaders/vertex_shader.glsl");
    std::string fragmentSource = Shader::readShaderSource("src/Shaders/fragment_shader.glsl");
    GLuint shaderProgram = Shader::createShaderProgram(vertexSource, fragmentSource);

    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glUseProgram(shaderProgram);

    glm::vec3 cameraPos = glm::vec3(0.0f, 40.0f, 0.0f);
    glm::vec3 cameraFront = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    glm::mat4 view;

    sf::Mouse mouse;
    window.requestFocus();
    window.setMouseCursorVisible(false);
    window.setMouseCursorGrabbed(true);
    sf::Vector2i centerPosition(window.getPosition().x + window.getSize().x / 2, window.getPosition().y + window.getSize().y / 2);
    sf::Vector2i windowCenter(window.getSize().x / 2, window.getSize().y / 2);
    mouse.setPosition(windowCenter, window);

    float lastX = windowCenter.x, lastY = windowCenter.y;
    bool firstMouse = true;
    float yaw = 0.0f; // yaw = rotation
    float pitch = 0.0f; // pitch = inclinaison
    float sensitivity = 0.1f;

    PerlinNoise perlin;

    int chunkSize = 16;
    double scale = 0.05; 
    double heightMultiplier = 40.0;  // height max
    double heightOffset = 30.0; // height normal
    

    Coord initialChunkCoord = {0, 0, 0};
    updateChunksAroundCamera(perlin, Coord(0,0,0), scale, heightMultiplier, heightOffset);
    chunkCoordAlreadyGenerated.push_back(initialChunkCoord);
        
    while (window.isOpen()) {
        float cameraSpeed = 0.90f;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z)) cameraPos += cameraSpeed * cameraFront;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) cameraPos -= cameraSpeed * cameraFront;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::E)) cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2)) cameraPos += cameraSpeed * cameraUp;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Tab)) cameraPos -= cameraSpeed * cameraUp;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) window.close();

        bool ignoreMouse = false;
        sf::Event event;
        while (window.pollEvent(event)) {
            switch (event.type) {
                case sf::Event::Closed:
                    window.close();
                    break;
                case sf::Event::MouseMoved:
                    if (ignoreMouse) {
                        ignoreMouse = false;
                        break;
                    }

                    if (!firstMouse) {
                        float xOffset = event.mouseMove.x - lastX;
                        float yOffset = lastY - event.mouseMove.y;

                        yaw += xOffset * sensitivity;
                        pitch += yOffset * sensitivity;

                        if(pitch > 89.0f)
                            pitch = 89.0f;
                        if(pitch < -89.0f)
                            pitch = -89.0f;

                        glm::vec3 front;
                        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
                        front.y = sin(glm::radians(pitch));
                        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
                        cameraFront = glm::normalize(front);

                    } else {
                        firstMouse = false;
                    }

                    mouse.setPosition(centerPosition, window);
                    ignoreMouse = true;

                    lastX = centerPosition.x;
                    lastY = centerPosition.y;
                    break;
                case sf::Event::Resized:
                    glViewport(0, 0, event.size.width, event.size.height);
                    break;
            }
        }

        text.setString("X: " + std::to_string(cameraPos.x) + " Y: " + std::to_string(cameraPos.y) + " Z: " + std::to_string(cameraPos.z));

        window.clear();

        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), float(window.getSize().x) / window.getSize().y, 0.1f, 100.0f);

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        Frustum frustum;
        glm::mat4 viewProjection = projection * view;
        frustum.update(viewProjection);

        Coord cameraChunkCoord = {
            static_cast<int>(floor(cameraPos.x / CHUNK_SIZE) * CHUNK_SIZE),
            0,
            static_cast<int>(floor(cameraPos.z / CHUNK_SIZE) * CHUNK_SIZE)
        };

        if(std::find(chunkCoordAlreadyGenerated.begin(), chunkCoordAlreadyGenerated.end(), cameraChunkCoord) == chunkCoordAlreadyGenerated.end()) {
            updateChunksAroundCamera(perlin, cameraChunkCoord, scale, heightMultiplier, heightOffset);
        }
        
        for (const auto& pair : blocMap) {
            const Bloc* bloc = pair.second;
            if (bloc != nullptr && frustum.intersects(bloc->getMinBounds(), bloc->getMaxBounds())) {
                bloc->Draw(shaderProgram);
            }
        }


        while (glGetError() != GL_NO_ERROR) {} //TODO
        window.pushGLStates();
        window.draw(text);
        window.popGLStates();


        window.display();
    }
    
    blocMap.clear();
    glDeleteProgram(shaderProgram);

    return 0;
}
