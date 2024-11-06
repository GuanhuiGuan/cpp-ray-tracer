#ifndef GEO_SPHERE_H
#define GEO_SPHERE_H

#include "hittable.h"

namespace krt {

class Sphere : public Hittable {
public:
    Point center;
    double radius;
    std::shared_ptr<BaseMat> mat;

    Sphere(const Point& p, const double& r, const std::shared_ptr<BaseMat> m) : center {p}, radius {r}, mat {m} {}

    bool hit(const Ray& ray, Interval tInterval, HitRecord& record) const override {
        Vec3 oc = center - ray.origin;
        double a = ray.dir.lengthSquared();
        double h = dot(ray.dir, oc);
        double c = oc.lengthSquared() - radius * radius;
        double discriminant = h * h - a * c;

        if (discriminant < 0) return false;

        double discriminantRoot = std::sqrt(discriminant);
        double t = (h - discriminantRoot) / a; // try the closer one first
        if (!tInterval.surrounds(t)) {
            t = (h + discriminantRoot) / a; // try the other solution
            if (!tInterval.surrounds(t)) return false;
        }

        record.hitPoint = ray.at(t);
        record.t = t;
        Vec3 outwardNormal = (record.hitPoint - center) / radius;
        record.setFaceNormal(ray, outwardNormal);
        record.material = mat;
        return true;
    }
};

}

#endif