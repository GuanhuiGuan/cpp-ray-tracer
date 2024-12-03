#ifndef MAT_LIGHT_H
#define MAT_LIGHT_H

#include "common.h"

namespace krt {

class DiffuseLight : public BaseMat {
    double amplifier;
public:

    DiffuseLight(const Color& albedo, double amplifier = 1) : DiffuseLight(std::make_shared<TexSolidColor>(albedo), amplifier) {}

    DiffuseLight(const std::shared_ptr<Texture>& t, double amplifier = 1) : amplifier {amplifier} {
        tex = t;
    }

    Color emit(const double& u, const double& v, const Point& point) const override {
        return tex->color(u, v, point) * amplifier;
    }
};

}

#endif