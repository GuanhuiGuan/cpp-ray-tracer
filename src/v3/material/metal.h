#ifndef MAT_METAL_H
#define MAT_METAL_H

#include "common.h"

namespace krt {

class Metal : public BaseMat {

    double fuzzy;

public:

    Metal (const Color& albedo, const double f = 0) : Metal(std::make_shared<TexSolidColor>(albedo), f) {}

    Metal (const std::shared_ptr<Texture>& t, const double f = 0) : fuzzy {f} {tex = t;}

    bool scatter(const Ray& ray, HitRecord& hRec, ScatterRecord& sRec) const override {
        sRec.attenuation = tex->color(hRec.u, hRec.v, hRec.hitPoint);
        sRec.skipPdf = true;
        sRec.pdf = nullptr;
        sRec.skipPdfRay = {
            hRec.hitPoint,
            reflect(ray.dir, hRec.normal).unitVec() + fuzzy * Vec3::genRandomUnitVec(),
            ray.time,
        };
        return true;
    }
};

}

#endif