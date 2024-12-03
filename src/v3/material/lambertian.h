#ifndef MAT_LAMBERTIAN_H
#define MAT_LAMBERTIAN_H

#include "common.h"

namespace krt {

class Lambertian : public BaseMat {

public:

    Lambertian(const Color& albedo) : Lambertian(std::make_shared<TexSolidColor>(albedo)) {}

    Lambertian(const std::shared_ptr<Texture>& t) {tex = t;}

    bool scatter(const Ray& ray, HitRecord& hRec, ScatterRecord& sRec) const override {
        sRec.attenuation = tex->color(hRec.u, hRec.v, hRec.hitPoint);
        sRec.skipPdf = false;
        sRec.pdf = std::make_shared<CosinePdf>(hRec.normal);
        return true;
    }

    double scatterPdf(const Ray& inRay, const Ray& outRay, const HitRecord& hRec) const override {
        double cosTheta {dot(outRay.dir.unitVec(), hRec.normal)};
        return cosTheta <= 0 ? 0 : cosTheta / pi;
    }
};

}

#endif