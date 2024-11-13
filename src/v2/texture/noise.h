#ifndef TEXTURE_NOISE_H
#define TEXTURE_NOISE_H

#include "texture.h"
#include "../infra/perlin.h"

namespace krt {

class Noise : public Texture {
    double scale;
    int turbDepth;
    Color baseColor;
    Perlin perlin{};
public:
    Noise(double _scale, int depth = 7, const Color& color = Color{0.5, 0.5, 0.5}) 
    : scale {_scale}, turbDepth {depth}, baseColor {color} {}

    Color color(const double& u, const double& v, const Point& p) const {
        return baseColor * (1 + std::sin(scale * p.z() + 10 * perlin.turbulence(p, turbDepth)));
    }
};

}

#endif