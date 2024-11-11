#ifndef MAT_DIELECTRIC_H
#define MAT_DIELECTRIC_H

#include "common.h"
#include "../infra/export.h"

namespace krt {

class Dielectric : public BaseMat {

    double relRefractionIdx; // relative refraction index, internalRefractionIdx / externalRefractionIdx

public:

    Dielectric (const Color& a, const double ri) : relRefractionIdx {ri} {albedo = a;}

    bool scatter(const Ray& ray, HitRecord& record, Ray& outRay, Color& attenuation) const override {
        attenuation = albedo;
        outRay.origin = record.hitPoint;
        outRay.time = ray.time;
        double ri = record.frontFacing ? 1 / relRefractionIdx : relRefractionIdx; // etaOut / etaIn
        Vec3 unitInDir = ray.dir.unitVec();
        double cosIn = std::min(dot(-unitInDir, record.normal), 1.0); // avoid sqrt error
        double sinIn = std::sqrt(1.0 - cosIn * cosIn);

        bool cannotRefract = ri * sinIn > 1;
        if (cannotRefract || reflectance(cosIn, ri) > randomDouble()) {
            // reflect
            outRay.dir = reflect(unitInDir, record.normal);
        } else {
            // refract
            outRay.dir = refract(unitInDir, record.normal, ri);
        }
        return true;
    }

    static double reflectance(const double cosIn, const double relRi) {
        double r0 = std::pow((1 - relRi) / (1 + relRi), 2);
        return r0 + (1 - r0) * std::pow(1 - cosIn, 5);
    }
};

}

#endif