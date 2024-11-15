#ifndef MAT_LIGHT_H
#define MAT_LIGHT_H

#include "common.h"
#include "../infra/export.h"

namespace krt {

class DiffuseLight : public BaseMat {

public:

    DiffuseLight(const Color& albedo) : DiffuseLight(std::make_shared<TexSolidColor>(albedo)) {}

    DiffuseLight(const std::shared_ptr<Texture>& t) {
        tex = t;
    }

    Color emit(const double& u, const double& v, const Point& point) const override {
        return tex->color(u, v, point);
    }
};

}

#endif