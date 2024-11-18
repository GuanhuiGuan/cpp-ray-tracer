#ifndef MIX_VOLUME_H
#define MIX_VOLUME_H

#include "../material/export.h"
#include "../geometry/export.h"

namespace krt {

class ConstMedium : public Hittable {
    std::shared_ptr<Hittable> boundary;
    double density;
    double negInvDensity;
    std::shared_ptr<BaseMat> phaseFunc;
public:
    ConstMedium(std::shared_ptr<Hittable> boundary, double density, std::shared_ptr<BaseMat> phaseFunc) 
    : boundary {boundary}, density {density}, negInvDensity {-1.0 / density}, phaseFunc {phaseFunc} {}

    ConstMedium(std::shared_ptr<Hittable> boundary, double density, std::shared_ptr<Texture> tex) 
    : ConstMedium(boundary, density, std::make_shared<Isotropic>(tex)) {}

    ConstMedium(std::shared_ptr<Hittable> boundary, double density, const Color& color) 
    : ConstMedium(boundary, density, std::make_shared<TexSolidColor>(color)) {}

    bool hit(const Ray& ray, Interval tInterval, HitRecord& record) const override {
        HitRecord rec0 {};
        if (!boundary->hit(ray, Interval::universe, rec0)) return false;
        HitRecord rec1 {};
        if (!boundary->hit(ray, Interval {rec0.t + 1e-4, infinity}, rec1)) return false;

        if (rec0.t < tInterval.min) rec0.t = tInterval.min;
        if (rec1.t > tInterval.max) rec1.t = tInterval.max;
        if (rec0.t >= rec1.t) return false;
        rec0.t = std::max(rec0.t, 0.0);

        double rayLen {ray.dir.length()};
        double distInBoundary {rayLen * (rec1.t - rec0.t)};
        double hitDist {negInvDensity * std::log(randomDouble())};
        if (hitDist > distInBoundary) return false;

        record.t = rec0.t + hitDist / rayLen;
        record.hitPoint = ray.at(record.t);

        record.normal = Vec3{0, 1, 0}; // arbitary
        record.frontFacing = true; // arbitary
        record.material = phaseFunc;
        return true;
    }

    const Aabb& boundingBox() const override {
        return boundary->boundingBox();
    }
};

}

#endif