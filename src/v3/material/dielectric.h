#ifndef MAT_DIELECTRIC_H
#define MAT_DIELECTRIC_H

#include "common.h"

namespace krt {

class Dielectric : public BaseMat {

    double relRefractionIdx; // relative refraction index, internalRefractionIdx / externalRefractionIdx

public:

    Dielectric(const Color& albedo, const double ri) : Dielectric(std::make_shared<TexSolidColor>(albedo), ri) {}

    Dielectric(const std::shared_ptr<Texture>& t, const double ri) : relRefractionIdx {ri} {tex = t;}

    bool scatter(const Ray& ray, HitRecord& hRec, ScatterRecord& sRec) const override {
        sRec.attenuation = tex->color(hRec.u, hRec.v, hRec.hitPoint);
        sRec.skipPdf = true;
        
        double ri = hRec.frontFacing ? 1 / relRefractionIdx : relRefractionIdx; // etaOut / etaIn
        Vec3 unitInDir = ray.dir.unitVec();
        double cosIn = std::min(dot(-unitInDir, hRec.normal), 1.0); // avoid sqrt error
        double sinIn = std::sqrt(1.0 - cosIn * cosIn);

        bool cannotRefract = ri * sinIn > 1;
        Vec3 outDir;
        if (cannotRefract || reflectance(cosIn, ri) > randomDouble()) {
            // reflect
            outDir = reflect(unitInDir, hRec.normal);
        } else {
            // refract
            outDir = refract(unitInDir, hRec.normal, ri);
        }
        sRec.skipPdfRay = {
            hRec.hitPoint,
            outDir,
            ray.time,
        };
        return true;
    }

    static double reflectance(const double cosIn, const double relRi) {
        double r0 = std::pow((1 - relRi) / (1 + relRi), 2);
        return r0 + (1 - r0) * std::pow(1 - cosIn, 5);
    }
};

}

#endif