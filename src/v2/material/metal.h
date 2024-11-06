#ifndef MAT_METAL_H
#define MAT_METAL_H

#include "common.h"
#include "../infra/export.h"

namespace krt {

class Metal : public BaseMat {

    double fuzzy;

public:

    Metal (const Color& a, const double f) : fuzzy {f} {albedo = a;}

    bool scatter(const Ray& ray, HitRecord& record, Ray& outRay, Color& attenuation) const override {
        outRay.origin = record.hitPoint;
        outRay.dir = reflect(ray.dir, record.normal).unitVec() + fuzzy * Vec3::genRandomUnitVec();
        attenuation = albedo;
        return dot(outRay.dir, record.normal) > 0;
    }
};

}

#endif