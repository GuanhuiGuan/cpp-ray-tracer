#ifndef MIX_REC_H
#define MIX_REC_H

#include "../infra/export.h"
#include "../texture/texture.h"

namespace krt {

class BaseMat; // forward declaration

class ScatterRecord {
public:
    bool skipPdf;
    Ray skipPdfRay;
    Color attenuation;
    std::shared_ptr<Pdf> pdf;
};

class HitRecord {
public:
    Point hitPoint;
    double t;
    Vec3 normal;
    bool frontFacing;
    double u; // u of texture map
    double v; // v of texture map
    std::shared_ptr<BaseMat> material;

    // here we assume the ray always go against the face normal
    // outwardNormal is assumed a unit vector
    void setFaceNormal(const Ray& ray, const Vec3& outwardNormal) {
        frontFacing = dot(ray.dir, outwardNormal) < 0;
        normal = frontFacing ? outwardNormal : -outwardNormal;
    }
};

}

#endif