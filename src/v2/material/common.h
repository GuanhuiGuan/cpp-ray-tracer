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

    virtual Color emit(const double& u, const double& v, const Point& point) const {return Color{};}

    virtual std::ostream& log(std::ostream& os) const {
        os << "{albedo=" << albedo
            << "}";
        return os;
    }
};

std::ostream& operator<<(std::ostream& os, const BaseMat& mat) {
    return mat.log(os);
}

}

#endif