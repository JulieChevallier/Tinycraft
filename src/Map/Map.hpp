#ifndef MAP_H
#define MAP_H

#include <unordered_map>
#include <vector>
#include <tuple>
#include <memory>
#include <cmath>
#include <algorithm>
#include "../Blocs/Bloc.hpp" 
#include "../PerlinNoise/PerlinNoise.hpp" 

using BlocPtr = std::unique_ptr<Bloc>;
using Chunk = std::vector<BlocPtr>;
using Coord = std::tuple<int, int, int>;

struct CoordHash {
    std::size_t operator()(const std::tuple<int, int, int>& coord) const noexcept;
};

extern std::unordered_map<Coord, Chunk, CoordHash> chunkMap;
extern std::unordered_map<Coord, Bloc*, CoordHash> blocMap;
extern std::vector<Coord> chunkCoordAlreadyGenerated;
extern std::vector<Coord> listeBlocAVerif;
extern Coord cameraChunkCoord;

void addBlocToMap(Bloc* bloc);
void generateWater(int x, int y, int z);
void smoothChunk(std::vector<Coord>& listeBlocAVerif);
bool canPlaceTree(int x, int y, int z);
bool isValidLocation(int x, int y, int z);
std::vector<Coord> generateLeavesCoords(int x, int y, int z);
void generateTree(int x, int y, int z);
Chunk generateChunk(PerlinNoise& perlin, Coord startCoord, Coord endCoord, double scale, double heightMultiplier, double heightOffset);
void updateChunksAroundCamera(PerlinNoise& perlin, Coord cameraChunkCoord, double scale, double heightMultiplier, double heightOffset);

#endif // MAP_H
