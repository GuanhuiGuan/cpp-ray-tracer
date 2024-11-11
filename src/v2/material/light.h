#ifndef MAT_LIGHT_H
#define MAT_LIGHT_H

#include "common.h"
#include "../infra/export.h"

namespace krt {

class DiffuseLight : public BaseMat {

public:

    DiffuseLight (const Color& a) {
        albedo = a;
    }

    bool scatter(const Ray& ray, HitRecord& record, Ray& outRay, Color& attenuation) const override {
        attenuation = albedo;
        return true;
    }

    Color emit(const double& u, const double& v, const Point& point) const override {return albedo;}
};

}

#endif