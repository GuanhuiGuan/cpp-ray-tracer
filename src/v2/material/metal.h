#ifndef MAT_METAL_H
#define MAT_METAL_H

#include "common.h"
#include "../infra/export.h"

namespace krt {

class Metal : public BaseMat {

    double fuzzy;

public:

    Metal (const Color& albedo, const double f = 0) : Metal(std::make_shared<TexSolidColor>(albedo), f) {}

    Metal (const std::shared_ptr<Texture>& t, const double f = 0) : fuzzy {f} {tex = t;}

    bool scatter(const Ray& ray, HitRecord& record, Ray& outRay, Color& attenuation) const override {
        outRay.origin = record.hitPoint;
        outRay.dir = reflect(ray.dir, record.normal).unitVec() + fuzzy * Vec3::genRandomUnitVec();
        outRay.time = ray.time;
        attenuation = tex->color(record.u, record.v, record.hitPoint);
        return dot(outRay.dir, record.normal) > 0;
    }
};

}

#endif