#ifndef MAT_LAMBERTIAN_H
#define MAT_LAMBERTIAN_H

#include "common.h"

namespace krt {

class Lambertian : public BaseMat {

public:

    Lambertian (const Color& a) {albedo = a;}

    bool scatter(const Ray& ray, HitRecord& record, Ray& outRay, Color& attenuation) const override {
        Vec3 outDir = record.normal + Vec3::genRandomUnitVec();
        outRay.origin = record.hitPoint;
        outRay.dir = outDir;
        attenuation = albedo;
        return true;
    }
};

}

#endif