# Tinycraft
A little minecraft for my own pleasure

TinyCraft is a 3D sandbox game developed using C++ and OpenGL, with a focus on generating and rendering a voxel-based world.

![TinyCraft before texture](Ressources/readme_V1.png)
![TinyCraft mountains](Ressources/readme_V2_1.png)
![TinyCraft island](Ressources/readme_V2_2.png)

### Implemented Features

- **Procedural Generation Algorithm (Perlin Noise)**: Generates terrain using Perlin noise for natural-looking landscapes.
- **Hashmap Optimization**: Utilizes hashmaps for efficient data storage and retrieval.
- **Frustum Culling**: Implements frustum culling to optimize rendering by not drawing blocks outside the camera's view.
- **Texture Manager**: Manages textures to avoid regenerating textures for each block creation.
- **Chunk Management**: Manages 16x16 chunks with progressive map generation based on camera movement.
- **Various Block Types**: Includes different block types (grass, dirt, stone, sand, water, wood, foliage) using polymorphism.
- **Tree Generation**: Generates trees (currently under correction).
- **Camera Movement**: Allows free camera movement within the world.
- **Multiple Biomes**: Features different biomes, including islands and mountains.
- **Visual Coordinate System**: Implements a visual coordinate system for navigation.

### Planned Features

- **Multithreading**: Improve performance by parallelizing tasks.
- **Minimap**: Add a minimap to better visualize the effects of Perlin noise.
- **Native Biome Generation**: Implement native biome generation for more diverse environments.
- **Player Interaction**: Add player interactions such as breaking/placing blocks and survival mode movement with collision detection.
- **Chunk Loading Management**: Optimize chunk loading to display only chunks within the player's range.
- **Code Refactoring**: Reorganize the codebase for better maintainability.

## Prerequisites

Before you can compile and run TinyCraft, you need to have the following software and libraries installed:

1. **GNU Compiler Collection (GCC)**: Ensure you have `g++` installed. You can install it using your package manager:
    ```sh
    sudo apt-get install g++
    ```

2. **SFML (Simple and Fast Multimedia Library)**: TinyCraft uses SFML for window management and graphics. Install SFML with:
    ```sh
    sudo apt-get install libsfml-dev
    ```

3. **GLEW (OpenGL Extension Wrangler Library)**: Install GLEW to manage OpenGL extensions:
    ```sh
    sudo apt-get install libglew-dev
    ```

4. **GLU (OpenGL Utility Library)**: Install GLU for additional OpenGL utilities:
    ```sh
    sudo apt-get install libglu1-mesa-dev
    ```

5. **SOIL (Simple OpenGL Image Library)**: Install SOIL for image loading:
    ```sh
    sudo apt-get install libsoil-dev
    ```

6. **CMake** (optional, but recommended for managing the build process):
    ```sh
    sudo apt-get install cmake
    ```

## Installation

1. **Clone the repository**:
    ```sh
    git clone https://github.com/JulieChevallier/TinyCraft.git
    cd TinyCraft
    ```

2. **Build and run the project** using the provided Makefile:
    ```sh
    make run
    ```

## The project's source code is organized into several directories:

* **src/**: Contains the main source files.
* **src/Shaders/**: Contains shader source files.
* **src/Blocs/**: Contains code for different block types.
* **src/PerlinNoise/**: Contains code for generating Perlin noise.
* **src/Texture/**: Contains texture handling code.
* **src/Tools/**: Contains utility functions and frustum culling logic.
* **src/Map/**: Contains map generation and handling code.
* **src/Ressources/**: Contains resource files such as fonts and images.

## Contributing
If you wish to contribute to this project, please fork the repository and use a feature branch. Pull requests are welcome.

## Acknowledgements

- [SFML](https://www.sfml-dev.org/)
- [GLEW](http://glew.sourceforge.net/)
- [GLU](https://www.opengl.org/resources/libraries/glu/)
- [SOIL](http://www.lonesock.net/soil.html)
