#include <GL/glew.h>
#include <GLFW/glfw3.h>
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
#include <algorithm>
#include "stb_image.h"
#include "../Shaders/Shader.hpp"
#include "../Texture/TextureManager.hpp"
#include "../Map/Map.hpp"
#include "FrustumCulling.hpp"

const int CHUNK_SIZE = 16;
const int NUM_CHUNKS_PER_SIDE = 3;