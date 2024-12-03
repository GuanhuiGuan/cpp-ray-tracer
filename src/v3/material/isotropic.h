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

    bool scatter(const Ray& ray, HitRecord& hRec, ScatterRecord& sRec) const override {
        sRec.attenuation = tex->color(hRec.u, hRec.v, hRec.hitPoint);
        sRec.skipPdf = false;
        sRec.pdf = std::make_shared<SpherePdf>();
        return true;
    }


    double scatterPdf(const Ray& inRay, const Ray& outRay, const HitRecord& hRec) const override {
        // uniform sphere
        return 0.25 * pi;
    }
};

}

#endif