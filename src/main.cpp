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

using Chunk = std::vector<Bloc>;
using Coord = std::pair<int, int>;

const int CHUNK_SIZE = 16;
const int NUM_CHUNKS_PER_SIDE = 3;

std::vector<Chunk> chunks;

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

void generateAllChunks(PerlinNoise& perlin, int centerX, int centerZ, double scale, double heightMultiplier, double heightOffset) {
    int offset = (NUM_CHUNKS_PER_SIDE - 1) / 2 * CHUNK_SIZE; 

    for (int i = -offset; i <= offset; i += CHUNK_SIZE) {
        for (int j = -offset; j <= offset; j += CHUNK_SIZE) {
            Coord startCoord = {centerX + i, centerZ + j};
            Coord endCoord = {startCoord.first + CHUNK_SIZE - 1, startCoord.second + CHUNK_SIZE - 1};
            chunks.push_back(generateChunk(perlin, startCoord, endCoord, scale, heightMultiplier, heightOffset));
        }
    }
}

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
    double heightMultiplier = 40.0;  // hauteur max
    double heightOffset = 30.0; // hauteur de base
    
    generateAllChunks(perlin, 0, 0, scale, heightMultiplier, heightOffset);
    
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

        // Dessin des cubes
        for (const auto& chunk : chunks) {
            for (const auto& bloc : chunk) {
                bloc.Draw(shaderProgram);
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
