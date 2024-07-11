#include "Tools/Tools.cpp"

int main() {

    // Manage text display
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

    // Create window
    sf::RenderWindow window(sf::VideoMode(1600, 1200), "TinyCraft", sf::Style::Default, sf::ContextSettings(24));
    window.setFramerateLimit(60);
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    // Load shaders
    std::string vertexSource = Shader::readShaderSource("src/Shaders/vertex_shader.glsl");
    std::string fragmentSource = Shader::readShaderSource("src/Shaders/fragment_shader.glsl");
    GLuint shaderProgram = Shader::createShaderProgram(vertexSource, fragmentSource);


    glEnable(GL_DEPTH_TEST); // Enable depth test
    glEnable(GL_BLEND); // Enable blending 
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Set blending function 
    glCullFace(GL_BACK); // Cull back face of polygons for culing
    glFrontFace(GL_CCW); // Front face is counter-clockwise oriented polygon
    glUseProgram(shaderProgram); // Use shader program
    stbi_set_flip_vertically_on_load(true); // Flip texture vertically

    // Camera parameters
    glm::vec3 cameraPos = glm::vec3(0.0f, 40.0f, 0.0f);
    glm::vec3 cameraFront = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

    glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    glm::mat4 view;

    // Mouse parameters
    sf::Mouse mouse;
    window.requestFocus();
    window.setMouseCursorVisible(false);
    window.setMouseCursorGrabbed(true);
    sf::Vector2i centerPosition(window.getPosition().x + window.getSize().x / 2, window.getPosition().y + window.getSize().y / 2);
    sf::Vector2i windowCenter(window.getSize().x / 2, window.getSize().y / 2);
    mouse.setPosition(windowCenter, window);

    // Perlin noise parameters
    float lastX = windowCenter.x, lastY = windowCenter.y;
    bool firstMouse = true;
    float yaw = 0.0f; // yaw = rotation
    float pitch = 0.0f; // pitch = inclinaison
    float sensitivity = 0.1f;

    PerlinNoise perlin;

    int chunkSize = 16;
    double scale = 0.05;
    // Generation with lot of water
    double heightMultiplier = 19.0;  // height max
    double heightOffset = 19.0; // height normal
    // Generation with lot of mountains
    // double heightMultiplier = 30.0;  // height max
    // double heightOffset = 30.0; // height normal

    // Generate initial chunk
    Coord initialChunkCoord = {0, 0, 0};
    updateChunksAroundCamera(perlin, Coord(0,0,0), scale, heightMultiplier, heightOffset);
    chunkCoordAlreadyGenerated.push_back(initialChunkCoord);
        
    while (window.isOpen()) {
        float cameraSpeed = 0.90f; // Move speed

        // Player input management
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

        // Update text to display coordinates of the camera
        text.setString("X: " + std::to_string(cameraPos.x) + " Y: " + std::to_string(cameraPos.y) + " Z: " + std::to_string(cameraPos.z));

        window.clear();

        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));

        glClearColor(0.4f, 0.7f, 0.9f, 1.0f); // Set background color
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), float(window.getSize().x) / window.getSize().y, 0.1f, 100.0f);

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        // Frustum culling
        Frustum frustum;
        glm::mat4 viewProjection = projection * view;
        frustum.update(viewProjection);

        // Check if the camera is in a new chunk
        cameraChunkCoord = {
            static_cast<int>(floor(cameraPos.x / CHUNK_SIZE) * CHUNK_SIZE),
            0,
            static_cast<int>(floor(cameraPos.z / CHUNK_SIZE) * CHUNK_SIZE)
        };

        if(std::find(chunkCoordAlreadyGenerated.begin(), chunkCoordAlreadyGenerated.end(), cameraChunkCoord) == chunkCoordAlreadyGenerated.end()) {
            updateChunksAroundCamera(perlin, cameraChunkCoord, scale, heightMultiplier, heightOffset);
        }
        
        // Draw chunks
        for (const auto& pair : blocMap) {
            const Bloc* bloc = pair.second;
            if (bloc != nullptr && frustum.intersects(bloc->getMinBounds(), bloc->getMaxBounds())) {
                glUniform1i(glGetUniformLocation(shaderProgram, "useTexture"), bloc->getUseTexture());
                bloc->Draw(shaderProgram);
            }
        }


        while (glGetError() != GL_NO_ERROR) {} //TODO
        window.pushGLStates();
        window.draw(text);
        window.popGLStates();


        window.display();
    }
    
    chunkMap.clear();
    blocMap.clear();
    glDeleteProgram(shaderProgram);

    return 0;
}