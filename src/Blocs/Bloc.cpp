#include "Bloc.hpp"
#include <GL/glew.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Constructor
// The constructor creates the VAO and VBO for the bloc and sets the vertices and texture coordinates. It also sets the color of the bloc if it doesn't use a texture
// x, y, z: position of the bloc
// useTexture: if the bloc should use a texture
Bloc::Bloc(float x, float y, float z, bool useTexture) : position(x, y, z), useTexture(useTexture) {   
    float vertices[] = {
        // positions          // texture coords
        // Front face
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        // Back face
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        // Left face
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        // Right face
         0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        // Bottom face
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        // Top face
        -0.5f,  0.5f, -0.5f,  0.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 0.0f
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    if (!useTexture) {
        setColor();
    }
}

// Destructor
// Deletes the VAO and VBO
Bloc::~Bloc() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void Bloc::setupTexture() {
}

// Sets the color of the bloc based on its y position
// The color is a gradient from blue to green to orange to red
void Bloc::setColor() {
    float normalizedY = (position.y + 20.0f) / 100.0f;
    if (normalizedY <= 0.25f) {
        color = glm::mix(glm::vec3(0.0f, 0.0f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f), normalizedY / 0.25f);
    } else if (normalizedY <= 0.5f) {
        color = glm::mix(glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f), (normalizedY - 0.25f) / 0.25f);
    } else if (normalizedY <= 0.75f) {
        color = glm::mix(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 0.5f, 0.0f), (normalizedY - 0.5f) / 0.25f);
    } else {
        color = glm::mix(glm::vec3(1.0f, 0.5f, 0.0f), glm::vec3(0.5f, 0.0f, 0.0f), (normalizedY - 0.75f) / 0.25f);
    }
}

// Draws the bloc
// shaderProgram: the shader program to use
void Bloc::Draw(GLuint shaderProgram) const {
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);

    if (useTexture) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glUniform1i(glGetUniformLocation(shaderProgram, "texture1"), 0);
    } else {
        glUniform3fv(glGetUniformLocation(shaderProgram, "color"), 1, glm::value_ptr(color));
    }

    glUniform1f(glGetUniformLocation(shaderProgram, "alpha"), getAlpha());

    glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}