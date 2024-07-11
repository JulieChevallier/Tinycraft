#include "Map.hpp"
#include "../Blocs/Dirt.hpp"
#include "../Blocs/Grass.hpp"
#include "../Blocs/Stone.hpp"
#include "../Blocs/Sand.hpp"
#include "../Blocs/Wood.hpp"
#include "../Blocs/Leaves.hpp"
#include "../Blocs/Water.hpp"
#include "../Blocs/Normal.hpp"

std::unordered_map<Coord, Chunk, CoordHash> chunkMap; // Map of chunks
std::unordered_map<Coord, Bloc*, CoordHash> blocMap; // Map of blocs
std::vector<Coord> chunkCoordAlreadyGenerated; // List of chunks already generated
std::vector<Coord> listeBlocAVerif; // List of blocs to verify for smoothing
Coord cameraChunkCoord = {0, 0, 0};

const int CHUNK_SIZE = 16; // Size of a chunk
const int NUM_CHUNKS_PER_SIDE = 5; // Number of chunks per side generate (3*3 chunks in total)

std::size_t CoordHash::operator()(const std::tuple<int, int, int>& coord) const noexcept {
    auto [x, y, z] = coord;
    std::size_t h1 = std::hash<int>{}(x);
    std::size_t h2 = std::hash<int>{}(y);
    std::size_t h3 = std::hash<int>{}(z);
    return h1 ^ (h2 << 1) ^ (h3 << 2);
}

// Add a bloc to the map
// bloc: the bloc to add
void addBlocToMap(Bloc* bloc) {
    int x = static_cast<int>(std::round(bloc->getPosition().x));
    int y = static_cast<int>(std::round(bloc->getPosition().y));
    int z = static_cast<int>(std::round(bloc->getPosition().z));
    Coord key(x, y, z);
    blocMap[key] = bloc;
}

// Generate water blocs at the bottom of the map below the y = 19 if there is no bloc
// x, y, z: the coordinates of the bloc
void generateWater(int x, int y, int z) {
    if (y < 19) {
        int i = 19;
        auto it = chunkMap.find(Coord(x, i, z));
        if (it == chunkMap.end() || it->second.empty()) {
            BlocPtr newBloc = std::make_unique<Water>(static_cast<float>(x), static_cast<float>(i), static_cast<float>(z));
            addBlocToMap(newBloc.get());
            chunkMap[Coord(x, i, z)].emplace_back(std::move(newBloc));
        }
    }
}

// Smoothing the terrain by adding blocs between the blocs
// listeBlocAVerif: the list of blocs to verify
// Add three blocs below the blocs in the list of blocs to verify
// TODO: Make an algorithm which is more useful
void smoothChunk(std::vector<Coord>& listeBlocAVerif) {
    std::vector<Coord> toRemove;

    for (const auto& entry : listeBlocAVerif) {
        int x = std::get<0>(entry);
        int y = std::get<1>(entry);
        int z = std::get<2>(entry);

        auto it = chunkMap.find(Coord(x, y, z));
        if (it != chunkMap.end()) {
            bool isSand = false;
            for (const auto& bloc : it->second) {
                if (bloc->getType() == BlocType::SAND) {
                    isSand = true;
                    break;
                }
            }
            for (int i = y - 3; i < y; ++i) {
                BlocPtr newBloc;
                if (isSand) {
                    newBloc = std::make_unique<Sand>(static_cast<float>(x), static_cast<float>(i), static_cast<float>(z));
                    addBlocToMap(newBloc.get());
                    chunkMap[Coord(x, i, z)].emplace_back(std::move(newBloc));
                    generateWater(x, i, z);
                } else if (i == y - 1) {
                    newBloc = std::make_unique<Dirt>(static_cast<float>(x), static_cast<float>(i), static_cast<float>(z));
                    addBlocToMap(newBloc.get());
                    chunkMap[Coord(x, i, z)].emplace_back(std::move(newBloc));
                } else {
                    newBloc = std::make_unique<Stone>(static_cast<float>(x), static_cast<float>(i), static_cast<float>(z));
                    addBlocToMap(newBloc.get());
                    chunkMap[Coord(x, i, z)].emplace_back(std::move(newBloc));
                }
            }

            toRemove.push_back(entry);
        }
    }

    for (const auto& rem : toRemove) {
        listeBlocAVerif.erase(std::remove(listeBlocAVerif.begin(), listeBlocAVerif.end(), rem), listeBlocAVerif.end());
    }
}

// Check if a tree can be placed at the given location
// x, y, z: the coordinates of the tree
// return: true if the tree can be placed, false otherwise
bool canPlaceTree(int x, int y, int z) {
    std::vector<Coord> treeCoords = {
        {x, y, z}, {x, y + 1, z}, {x, y + 2, z}, {x, y + 3, z},
        {x, y + 5, z}, {x + 1, y + 4, z}, {x + 1, y + 5, z}, {x - 1, y + 4, z}, {x - 1, y + 5, z}, {x, y + 4, z + 1}, {x, y + 5, z + 1}, {x, y + 4, z - 1}, {x, y + 5, z - 1}, {x + 1, y + 2, z},
        {x - 1, y + 2, z}, {x, y + 2, z + 1}, {x, y + 2, z - 1}, {x + 1, y + 3, z + 1}, {x + 1, y + 2, z + 1}, {x - 1, y + 3, z + 1}, {x - 1, y + 2, z + 1}, {x + 1, y + 3, z - 1},
        {x + 1, y + 2, z - 1}, {x - 1, y + 3, z - 1}, {x - 1, y + 2, z - 1}, {x + 2, y + 3, z}, {x + 2, y + 2, z}, {x - 2, y + 3, z}, {x - 2, y + 2, z}, {x, y + 3, z + 2}, {x, y + 2, z + 2}, {x, y + 3, z - 2}, {x, y + 3, z - 2},
        {x, y + 2, z - 2}, {x + 2, y + 3, z + 2}, {x + 2, y + 2, z + 2}, {x - 2, y + 3, z + 2}, {x - 2, y + 2, z + 2}, {x + 2, y + 3, z - 2}, {x + 2, y + 2, z - 2}, {x - 2, y + 2, z - 2}, {x - 2, y + 2, z - 1},
        {x - 2, y + 3, z - 1}, {x - 2, y + 2, z + 1}, {x - 2, y + 3, z + 1}, {x + 2, y + 2, z - 1}, {x + 2, y + 3, z - 1}, {x + 2, y + 2, z + 1}, {x + 2, y + 3, z + 1}, {x - 1, y + 2, z - 2}, {x - 1, y + 3, z - 2}, {x - 1, y + 2, z + 2},
        {x - 1, y + 3, z + 2}, {x + 1, y + 2, z - 2}, {x + 1, y + 3, z - 2}, {x + 1, y + 2, z + 2}, {x + 1, y + 3, z + 2}
    };

    for (const auto& coord : treeCoords) {
        if (chunkMap.find(coord) != chunkMap.end()) {
            return false;
        }
    }
    return true;
}

// Check if the location is valid
// bool isValidLocation(int x, int y, int z) {
//     int cameraX = std::get<0>(cameraChunkCoord);
//     int cameraZ = std::get<2>(cameraChunkCoord);

//     if (x < cameraX - CHUNK_SIZE || x > cameraX + CHUNK_SIZE * NUM_CHUNKS_PER_SIDE) {
//         return false;
//     }
//     if (z < cameraZ - CHUNK_SIZE || z > cameraZ + CHUNK_SIZE * NUM_CHUNKS_PER_SIDE) {
//         return false;
//     }
//     return true;
// }

// Generate the coordinates of the leaves of a tree
// x, y, z: the coordinates of the tree
// return: the coordinates of the leaves
std::vector<Coord> generateLeavesCoords(int x, int y, int z) {
    std::vector<Coord> leavesCoords = {
        {x, y + 5, z}, {x + 1, y + 4, z}, {x + 1, y + 5, z}, {x - 1, y + 4, z}, {x - 1, y + 5, z}, {x, y + 4, z + 1}, {x, y + 5, z + 1}, {x, y + 4, z - 1}, {x, y + 5, z - 1}, {x + 1, y + 2, z},
        {x - 1, y + 2, z}, {x, y + 2, z + 1}, {x, y + 2, z - 1}, {x + 1, y + 3, z + 1}, {x + 1, y + 2, z + 1}, {x - 1, y + 3, z + 1}, {x - 1, y + 2, z + 1}, {x + 1, y + 3, z - 1},
        {x + 1, y + 2, z - 1}, {x - 1, y + 3, z - 1}, {x - 1, y + 2, z - 1}, {x + 2, y + 3, z}, {x + 2, y + 2, z}, {x - 2, y + 3, z}, {x - 2, y + 2, z}, {x, y + 3, z + 2}, {x, y + 2, z + 2}, {x, y + 3, z - 2}, {x, y + 3, z - 2},
        {x, y + 2, z - 2}, {x + 2, y + 3, z + 2}, {x + 2, y + 2, z + 2}, {x - 2, y + 3, z + 2}, {x - 2, y + 2, z + 2}, {x + 2, y + 3, z - 2}, {x + 2, y + 2, z - 2}, {x - 2, y + 2, z - 2}, {x - 2, y + 2, z - 1},
        {x - 2, y + 3, z - 1}, {x - 2, y + 2, z + 1}, {x - 2, y + 3, z + 1}, {x + 2, y + 2, z - 1}, {x + 2, y + 3, z - 1}, {x + 2, y + 2, z + 1}, {x + 2, y + 3, z + 1}, {x - 1, y + 2, z - 2}, {x - 1, y + 3, z - 2}, {x - 1, y + 2, z + 2},
        {x - 1, y + 3, z + 2}, {x + 1, y + 2, z - 2}, {x + 1, y + 3, z - 2}, {x + 1, y + 2, z + 2}, {x + 1, y + 3, z + 2}
    };

    return leavesCoords;
}

// Generate a tree at the given location
// x, y, z: the coordinates of the tree
void generateTree(int x, int y, int z) {
    if (!canPlaceTree(x, y, z)) {
        return; 
    }

    for (int i = 0; i < 4; ++i) {
        BlocPtr wood = std::make_unique<Wood>(static_cast<float>(x), static_cast<float>(y + i), static_cast<float>(z));
        addBlocToMap(wood.get());
        chunkMap[Coord(x, y + i, z)].emplace_back(std::move(wood));
    }

    std::vector<Coord> leavesCoords = generateLeavesCoords(x, y, z);
    for (const auto& coord : leavesCoords) {
        BlocPtr leaves = std::make_unique<Leaves>(static_cast<float>(std::get<0>(coord)), static_cast<float>(std::get<1>(coord)), static_cast<float>(std::get<2>(coord)));
        addBlocToMap(leaves.get());
        chunkMap[coord].emplace_back(std::move(leaves));
    }
}

// Generate a chunk of the map with perlin noise
// perlin: the perlin noise generator
// startCoord: the starting coordinates of the chunk
// endCoord: the ending coordinates of the chunk
// scale: the scale of the perlin noise
// heightMultiplier: the height multiplier of the perlin noise
// heightOffset: the height offset of the perlin noise
// return: the generated chunk
// For each bloc of the chunk, generate a bloc with the perlin noise and add it to the chunkMap if it doesn't exist already
// If the height of the bloc is less than or equal to 20, generate a sand bloc, otherwise generate a grass bloc
// Add the bloc to the list of blocs to verify for smoothing
// Generate only the top layer of the chunk
Chunk generateChunk(PerlinNoise& perlin, Coord startCoord, Coord endCoord, double scale, double heightMultiplier, double heightOffset) {
    Chunk blocs;
    for (int x = 0; x < CHUNK_SIZE + 16; ++x) {
        for (int z = 0; z < CHUNK_SIZE; ++z) {
            int coordX = std::get<0>(startCoord) + x - 16;
            int coordZ = std::get<2>(startCoord) + z;
            double noise = perlin.noise(coordX * scale, 0.0, coordZ * scale);
            int height = static_cast<int>(noise * heightMultiplier + heightOffset);
            if (height <= 20){
                BlocPtr bloc = std::make_unique<Sand>(static_cast<float>(coordX), static_cast<float>(height), static_cast<float>(coordZ));
                addBlocToMap(bloc.get());
                blocs.emplace_back(std::move(bloc));
            }
            else {
                BlocPtr bloc = std::make_unique<Grass>(static_cast<float>(coordX), static_cast<float>(height), static_cast<float>(coordZ));
                addBlocToMap(bloc.get());
                blocs.emplace_back(std::move(bloc));

                // TODO: Generate tree without Erreur de segmentation
                if (coordX % 8 == 0 && coordZ % 8 == 0) {
                    generateTree(coordX, height, coordZ);
                }
            }
            
            listeBlocAVerif.push_back(Coord(coordX, height, coordZ));
            chunkMap[Coord(coordX, height, coordZ)] = std::move(blocs);
        }
    }
    return blocs;
}

// Update the chunks around the camera when the camera moves
// perlin: the perlin noise generator
// cameraChunkCoord: the coordinates of the chunk where the camera is
// scale: the scale of the perlin noise
// heightMultiplier: the height multiplier of the perlin noise
// heightOffset: the height offset of the perlin noise
// For each chunk around the camera, if it doesn't exist, generate it by calling generateChunk with coordinates of the chunk (start and end) and add it to the chunkMap
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
