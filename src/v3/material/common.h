#ifndef MAT_COMMON_H
#define MAT_COMMON_H

#include "../mixture/record.h"

namespace krt {

class HitRecord; // forward declaration to avoid circular ref

class BaseMat {
public:
    std::shared_ptr<Texture> tex;

    virtual ~BaseMat() = default;

    virtual bool scatter(const Ray& ray, HitRecord& hRec, ScatterRecord& sRec) const {
        return false;
    }

    virtual Color emit(const Ray& ray, HitRecord& hRec, const double& u, const double& v, const Point& point) const {return Color{};}

    virtual double scatterPdf(const Ray& inRay, const Ray& outRay, const HitRecord& hRec) const {
        return 0;
    }
};

}

#endif