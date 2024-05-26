#ifndef PERLINNOISE_HPP
#define PERLINNOISE_HPP

#include <vector>

class PerlinNoise {
public:
    PerlinNoise();
    double noise(double x, double y, double z) const;

private:
    std::vector<int> p; // permutation vector
    double fade(double t) const;
    double lerp(double t, double a, double b) const;
    double grad(int hash, double x, double y, double z) const;
};

#endif