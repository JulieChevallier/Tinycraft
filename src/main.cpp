#include <GL/glew.h>
#include <SFML/Window.hpp>
#include <SFML/OpenGL.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shaders/Shader.hpp"
// #include "Blocs/Bloc.hpp"
#include "Blocs/Dirt.hpp"

int main() {
    sf::Window window(sf::VideoMode(800, 600), "TinyCraft");
    window.setFramerateLimit(60);
    glewInit();

    // sources shaders
    std::string vertexSource = Shader::readShaderSource("src/Shaders/vertex_shader.glsl");
    std::string fragmentSource = Shader::readShaderSource("src/Shaders/fragment_shader.glsl");
    GLuint shaderProgram = Shader::createShaderProgram(vertexSource, fragmentSource);

    glEnable(GL_DEPTH_TEST);
    glUseProgram(shaderProgram);

    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    glm::mat4 view;


    // Souris
    sf::Mouse mouse;
    window.requestFocus();
    window.setMouseCursorVisible(false);
    window.setMouseCursorGrabbed(true);
    sf::Vector2i centerPosition(window.getPosition().x + window.getSize().x / 2, window.getPosition().y + window.getSize().y / 2);
    sf::Vector2i windowCenter(window.getSize().x / 2, window.getSize().y / 2);
    mouse.setPosition(windowCenter, window);

    float lastX = windowCenter.x, lastY = windowCenter.y;
    bool firstMouse = true;
    float yaw = -90.0f;
    float pitch = 0.0f;
    float sensitivity = 0.1f;

    Bloc::InitializeGeometry();

    while (window.isOpen()) {
        float cameraSpeed = 0.05f;

        // Bindings
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

                        // MAJ view camera
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

        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        glUseProgram(shaderProgram);

        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

        // matrice to shader
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        // Dessin du cube
        Dirt dirtBlock;
        dirtBlock.Draw(shaderProgram);

        window.display();
    }

    Bloc::CleanupGeometry();
    glDeleteProgram(shaderProgram);

    return 0;
}
// appuie Numpad1 = créatif
// appuie Numpad2 = survie