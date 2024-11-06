#ifndef MAT_LIGHT_H
#define MAT_LIGHT_H

#include "common.h"
#include "../infra/export.h"

namespace krt {

class Light : public BaseMat {

public:

    Light (const Color& a) {
        albedo = a;
    }

    bool scatter(const Ray& ray, HitRecord& record, Ray& outRay, Color& attenuation) const override {
        attenuation = albedo;
        return true;
    }

    Color emit() const override {return albedo;}
};

}

#endif