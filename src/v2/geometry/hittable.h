#ifndef GEO_HITTABLE_H
#define GEO_HITTABLE_H

#include "../infra/export.h"
#include "../material/common.h"
#include "aabb.h"

#include <vector>
#include <memory>

namespace krt {

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

class Hittable {
public:
    virtual ~Hittable() = default;

    virtual bool hit(const Ray& ray, Interval tInterval, HitRecord& record) const = 0;

    virtual const Aabb& boundingBox() const = 0;
};

class HittableList : public Hittable {
    std::vector<std::shared_ptr<Hittable>> list{};
    Aabb bBox;
public:
    HittableList() {}

    HittableList(std::shared_ptr<Hittable> ptr) {
        add(ptr);
    }

    void add(std::shared_ptr<Hittable> ptr) {
        list.push_back(ptr);
        bBox = Aabb(bBox, ptr->boundingBox());
    }

    void clear() {list.clear();}

    std::size_t size() const {return list.size();}

    std::vector<std::shared_ptr<Hittable>>& elements() {return list;}

    bool hit(const Ray& ray, Interval tInterval, HitRecord& record) const override {
        bool hit {false};
        double closestT {tInterval.max};
        HitRecord tempHitRecord;
        for (const std::shared_ptr<Hittable>& obj: list) {
            if (obj->hit(ray, Interval{tInterval.min, closestT}, tempHitRecord)) {
                hit = true;
                closestT = tempHitRecord.t;
                record = tempHitRecord;
            }
        }
        return hit;
    }

    const Aabb& boundingBox() const override {
        return bBox;
    }
};

}

#endif