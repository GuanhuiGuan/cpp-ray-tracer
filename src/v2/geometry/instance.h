#ifndef GEO_INSTANCE_H
#define GEO_INSTANCE_H

#include "hittable.h"

namespace krt {

class Translation : public Hittable {
    std::shared_ptr<Hittable> obj;
    Vec3 offset;
    Aabb bBox;
public:

    Translation(std::shared_ptr<Hittable> h, const Vec3& v) : obj {h}, offset {v} {
        bBox = h->boundingBox() + v;
    }

    const Aabb& boundingBox() const override {
        return bBox;
    }

    bool hit(const Ray& ray, Interval tInterval, HitRecord& record) const override {
        Ray offsetRay {ray.origin - offset, ray.dir, ray.time};
        if (!obj->hit(offsetRay, tInterval, record)) return false;
        record.hitPoint += offset;
        return true;
    }
};

// class Rotation : public Hittable {
//     std::shared_ptr<Hittable> obj;
//     double angle;
//     size_t axis;
//     Aabb bBox;
// public:

//     Rotation(std::shared_ptr<Hittable> h, double angle, size_t axis) : obj {h}, angle {angle}, axis {axis} {

//     }

//     const Aabb& boundingBox() const override {
//         return bBox;
//     }

//     bool hit(const Ray& ray, Interval tInterval, HitRecord& record) const override {
        
//     }
// };

}

#endif