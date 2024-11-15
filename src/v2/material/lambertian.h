#ifndef MAT_LAMBERTIAN_H
#define MAT_LAMBERTIAN_H

#include "common.h"

namespace krt {

class Lambertian : public BaseMat {

public:

    Lambertian(const Color& albedo) : Lambertian(std::make_shared<TexSolidColor>(albedo)) {}

    Lambertian(const std::shared_ptr<Texture>& t) {tex = t;}

    bool scatter(const Ray& ray, HitRecord& record, Ray& outRay, Color& attenuation) const override {
        Vec3 outDir = record.normal + Vec3::genRandomUnitVec();
        // catch degenerate scatter direction
        if (outDir.near0()) outDir = record.normal;
        outRay = Ray(record.hitPoint, outDir, ray.time);
        attenuation = tex->color(record.u, record.v, record.hitPoint);
        return true;
    }
};

}

#endif