#ifndef GEO_SPHERE_H
#define GEO_SPHERE_H

#include "hittable.h"
#include "math.h"

namespace krt {

class Sphere : public Hittable {
public:
    Point center;
    double radius;
    std::shared_ptr<BaseMat> mat;
    // returns where is the sphere center at the given time
    Point (*originAtTime)(const Sphere& sphere, const double& time) {[](const Sphere& sphere, const double& time) {return sphere.center;}};

    Vec3 radiusVec;

    Sphere(const Point& p, const double& r, const std::shared_ptr<BaseMat> m) 
    : center {p}, radius {r}, mat {m} {
        init();
    }

    Sphere(const Point& p, const double& r, const std::shared_ptr<BaseMat> m, 
        Point (*_originAtTime)(const Sphere& sphere, const double& time), double startTime, double endTime) 
    : center {p}, radius {r}, mat {m}, originAtTime{_originAtTime} {
        init(startTime, endTime);
    }

    bool hit(const Ray& ray, Interval tInterval, HitRecord& record) const override {
        Point centerNow = originAtTime(*this, ray.time);
        Vec3 oc = centerNow - ray.origin;
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
        Vec3 outwardNormal = (record.hitPoint - centerNow) / radius;
        record.setFaceNormal(ray, outwardNormal);
        record.material = mat;

        getUv(outwardNormal, record.u, record.v);

        return true;
    }

    const Aabb& boundingBox() const override {
        return bBox;
    }

private:
    Aabb bBox;

    void init(double startTime = 0, double endTime = 0) {
        radiusVec = radius * Vec3{1, 1, 1};
        bBox = getBBox(startTime, endTime);
    }

    // need to capture the entire motion
    // sample for simplicity
    Aabb getBBox(double startTime, double endTime) {
        Aabb box {getBBox(startTime)};
        box = Aabb{box, getBBox(endTime)};
        double delta {std::max((endTime - startTime) / 3, 1e-4)};
        for (double t = startTime + delta; t < endTime; t += delta) {
            box = Aabb(box, getBBox(t));
        }
        return box;
    }

    Aabb getBBox(double time) const {
        Point cNow {originAtTime(*this, time)};
        return Aabb {cNow - radiusVec, cNow + radiusVec};
    }

    // p: use unit outward normal
    static void getUv(const Point& p, double& u, double& v) {
        double phi {std::atan2(-p.z(), p.x()) + pi}; // angle starts from -X counter clockwise
        double theta {std::acos(-p.y())}; // angle starts from -Y to +Z
        u = phi / (2 * pi);
        v = theta / pi;
    }
};

}

#endif