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
#include "Shaders/Shader.hpp"
#include "Blocs/Bloc.hpp"
#include "PerlinNoise/PerlinNoise.hpp"
#include <unordered_map>
#include <functional>

using Chunk = std::vector<Bloc>;
using Coord = std::pair<int, int>;
struct CoordHash {
    std::size_t operator()(const Coord& coord) const {
        return std::hash<int>()(coord.first) ^ std::hash<int>()(coord.second);
    }
};

const int CHUNK_SIZE = 16;
const int NUM_CHUNKS_PER_SIDE = 3;

std::vector<Chunk> chunks;
std::unordered_map<Coord, Chunk, CoordHash> chunkMap;
std::vector<Coord> chunkCoordAlreadyGenerated;

// Allowing to generate a chunk of blocs
Chunk generateChunk(PerlinNoise& perlin, Coord startCoord, Coord endCoord, double scale, double heightMultiplier, double heightOffset) {
    Chunk blocs;
    for (int x = 0; x < CHUNK_SIZE + 16; ++x) {
        for (int z = 0; z < CHUNK_SIZE; ++z) {
            int coordX = startCoord.first + x - 16;
            int coordZ = startCoord.second + z;
            double noise = perlin.noise(coordX * scale, 0.0, coordZ * scale);
            int height = static_cast<int>(noise * heightMultiplier + heightOffset);
            blocs.emplace_back(static_cast<float>(coordX), static_cast<float>(height), static_cast<float>(coordZ));
        }
    }
    return blocs;
}

// Not used but for testing purposes
void generateAllChunks(PerlinNoise& perlin, int centerX, int centerZ, double scale, double heightMultiplier, double heightOffset) {
    int offset = (NUM_CHUNKS_PER_SIDE - 1) / 2 * CHUNK_SIZE; 

    for (int i = -offset; i <= offset; i += CHUNK_SIZE) {
        for (int j = -offset; j <= offset; j += CHUNK_SIZE) {
            Coord startCoord = {centerX + i, centerZ + j};
            Coord endCoord = {startCoord.first + CHUNK_SIZE - 1, startCoord.second + CHUNK_SIZE - 1};
            chunks.push_back(generateChunk(perlin, startCoord, endCoord, scale, heightMultiplier, heightOffset));
        }
    }
    // Test for a single chunk
    // chunks.push_back(generateChunk(perlin, Coord(0,0), Coord(15,15), scale, heightMultiplier, heightOffset));
}

// Update the chunks around the camera when the camera moves
void updateChunksAroundCamera(PerlinNoise& perlin, Coord cameraChunkCoord, double scale, double heightMultiplier, double heightOffset) {
    int chunkRadius = 1; 
    chunkCoordAlreadyGenerated.push_back(cameraChunkCoord);
    for (int i = -chunkRadius; i <= chunkRadius; ++i) {
        for (int j = -chunkRadius; j <= chunkRadius; ++j) {
            Coord neighborChunkCoord = {cameraChunkCoord.first + i * CHUNK_SIZE, cameraChunkCoord.second + j * CHUNK_SIZE};
            if (chunkMap.find(neighborChunkCoord) == chunkMap.end()){
                Coord endCoord = {neighborChunkCoord.first + CHUNK_SIZE - 1, neighborChunkCoord.second + CHUNK_SIZE - 1};
                chunks.push_back(generateChunk(perlin, neighborChunkCoord, endCoord, scale, heightMultiplier, heightOffset));
            }
        }
    }
}

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

    Coord initialChunkCoord = {0, 0};
    updateChunksAroundCamera(perlin, Coord(0,0), scale, heightMultiplier, heightOffset);
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
            static_cast<int>(floor(cameraPos.z / CHUNK_SIZE) * CHUNK_SIZE)
        };

        if(std::find(chunkCoordAlreadyGenerated.begin(), chunkCoordAlreadyGenerated.end(), cameraChunkCoord) == chunkCoordAlreadyGenerated.end()) {
            updateChunksAroundCamera(perlin, cameraChunkCoord, scale, heightMultiplier, heightOffset);
        }
        
        for (const auto& chunk : chunks) {
            for (const auto& bloc : chunk) {
                if (frustum.intersects(bloc.getMinBounds(), bloc.getMaxBounds())) {
                    bloc.Draw(shaderProgram);
                }
            }
        }

        while (glGetError() != GL_NO_ERROR) {} //TODO
        window.pushGLStates();
        window.draw(text);
        window.popGLStates();


        window.display();
    }

    glDeleteProgram(shaderProgram);

    return 0;
}
