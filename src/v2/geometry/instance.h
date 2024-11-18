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

class Rotation : public Hittable {
    std::shared_ptr<Hittable> obj;
    double angle;
    double sine;
    double cosine;
    size_t axis;
    Aabb bBox;
public:

    Rotation(std::shared_ptr<Hittable> h, double angle, size_t axis) : obj {h}, angle {angle}, axis {axis} {
        double theta {deg2Rad(angle)};
        sine = std::sin(theta);
        cosine = std::cos(theta);

        Point min {infinity, infinity, infinity};
        Point max {-infinity, -infinity, -infinity};
        const Aabb box {h->boundingBox()};
        for (int i {0}; i < 2; ++i) {
            for (int j {0}; j < 2; ++j) {
                for (int k {0}; k < 2; ++k) {
                    Point p {box[0][i], box[1][j], box[2][k]};
                    Point rp {rotate(p, sine, cosine, axis)};
                    min = minVec(min, rp);
                    max = maxVec(max, rp);
                }
            }
        }
        bBox = Aabb(min, max);
    }

    const Aabb& boundingBox() const override {
        return bBox;
    }

    bool hit(const Ray& ray, Interval tInterval, HitRecord& record) const override {
        Ray rotatedRay {rotate(ray.origin, -sine, cosine, axis), rotate(ray.dir, -sine, cosine, axis), ray.time};
        if (!obj->hit(rotatedRay, tInterval, record)) return false;
        record.hitPoint = rotate(record.hitPoint, sine, cosine, axis);
        record.normal = rotate(record.normal, sine, cosine, axis);
        return true;
    }
};

}

#endif