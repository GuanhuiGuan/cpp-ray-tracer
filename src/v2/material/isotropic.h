#ifndef MAT_ISOTROPIC_H
#define MAT_ISOTROPIC_H

#include "common.h"

namespace krt {

class Isotropic : public BaseMat {

public:

    Isotropic(const Color& albedo) : Isotropic(std::make_shared<TexSolidColor>(albedo)) {}

    Isotropic(const std::shared_ptr<Texture>& t) {
        tex = t;
    }

    bool scatter(const Ray& ray, HitRecord& record, Ray& outRay, Color& attenuation) const override {
        attenuation = tex->color(record.u, record.v, record.hitPoint);
        outRay = {record.hitPoint, Vec3::genRandomUnitVec(), ray.time};
        return true;
    }
};

}

#endif