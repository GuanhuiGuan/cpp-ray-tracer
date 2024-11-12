#ifndef MAT_COMMON_H
#define MAT_COMMON_H

#include "../infra/export.h"
#include "../texture/texture.h"

namespace krt {

class HitRecord; // forward declaration to avoid circular ref

class BaseMat {
public:
    std::shared_ptr<Texture> tex;

    virtual ~BaseMat() = default;

    virtual bool scatter(const Ray& ray, HitRecord& record, Ray& outRay, Color& attenuation) const {
        return false;
    }

    virtual Color emit(const double& u, const double& v, const Point& point) const {return Color{};}
};

}

#endif