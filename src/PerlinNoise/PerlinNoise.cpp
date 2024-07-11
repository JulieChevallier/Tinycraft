#include "PerlinNoise.hpp"
#include <cmath>
#include <random>
#include <algorithm>

// Constructor
// Initialize the permutation vector with a random engine and shuffle it to get a random permutation vector of 256 elements (0 to 255) and then duplicate it to avoid overflow when indexing the permutation vector in the noise function
PerlinNoise::PerlinNoise() {
    p.resize(256);
    std::iota(p.begin(), p.end(), 0);
    std::default_random_engine engine(std::random_device{}());
    std::shuffle(p.begin(), p.end(), engine);
    p.insert(p.end(), p.begin(), p.end());
}

// Calculate the noise value at a given position (x, y, z) in the 3D space
// x, y, z: position in the 3D space
// return: the noise value at the given position
double PerlinNoise::noise(double x, double y, double z) const {
    int X = static_cast<int>(std::floor(x)) & 255;
    int Y = static_cast<int>(std::floor(y)) & 255;
    int Z = static_cast<int>(std::floor(z)) & 255;

    x -= std::floor(x);
    y -= std::floor(y);
    z -= std::floor(z);

    double u = fade(x);
    double v = fade(y);
    double w = fade(z);

    int A = p[X] + Y;
    int AA = p[A] + Z;
    int AB = p[A + 1] + Z;
    int B = p[X + 1] + Y;
    int BA = p[B] + Z;
    int BB = p[B + 1] + Z;

    return lerp(w, lerp(v, lerp(u, grad(p[AA], x, y, z), grad(p[BA], x - 1, y, z)), lerp(u, grad(p[AB], x, y - 1, z), grad(p[BB], x - 1, y - 1, z))), lerp(v, lerp(u, grad(p[AA + 1], x, y, z - 1), grad(p[BA + 1], x - 1, y, z - 1)), lerp(u, grad(p[AB + 1], x, y - 1, z - 1), grad(p[BB + 1], x - 1, y - 1, z - 1))));
}

// return the fade of the noise
// t: value to fade
// return: the fade of the noise
double PerlinNoise::fade(double t) const {
    return t * t * t * (t * (t * 6 - 15) + 10); 
}

// return the linear interpolation of the noise
// t: value to interpolate
// a: first value
// b: second value
// return: the linear interpolation of the noise
double PerlinNoise::lerp(double t, double a, double b) const {
    return a + t * (b - a);
}

// return the gradient of the noise
// hash: hash value
// x, y, z: position in the 3D space
// return: the gradient of the noise
double PerlinNoise::grad(int hash, double x, double y, double z) const {
    int h = hash & 15;
    double u = h < 8 ? x : y;
    double v = h < 4 ? y : h == 12 || h == 14 ? x : z;
    return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}