#ifndef MAT_COMMON_H
#define MAT_COMMON_H

#include "../infra/export.h"

namespace krt {

class HitRecord; // forward declaration to avoid circular ref

class BaseMat {
public:

    Color albedo;

    virtual ~BaseMat() = default;

    virtual bool scatter(const Ray& ray, HitRecord& record, Ray& outRay, Color& attenuation) const {
        return false;
    }

    virtual Color emit() const {return Color{};}
};

}

#endif