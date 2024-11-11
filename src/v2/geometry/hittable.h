#ifndef GEO_HITTABLE_H
#define GEO_HITTABLE_H

#include "../infra/export.h"
#include "../material/common.h"
#include "aabb.h"

#include <vector>
#include <memory>

#define ENABLE_BVH

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

std::ostream& operator<<(std::ostream& os, const HitRecord& hitRecord) {
    os << "{hitPoint=" << hitRecord.hitPoint
        << ", t=" << hitRecord.t
        << ", normal=" << hitRecord.normal
        << ", frontFacing=" << hitRecord.frontFacing
        << ", u=" << hitRecord.u
        << ", v=" << hitRecord.v
    ;
    hitRecord.material ? (os << ", material=nullptr") : (os << ", material=" << *(hitRecord.material));
    os << "}";
    return os;
}

class Hittable {
public:
    virtual ~Hittable() = default;

    virtual bool hit(const Ray& ray, Interval tInterval, HitRecord& record) const = 0;

    virtual const Aabb& boundingBox() const = 0;

    virtual std::ostream& log(std::ostream& os) const {
        os << "{}";
        return os;
    }
};

std::ostream& operator<<(std::ostream& os, const Hittable& h) {
    return h.log(os);
}

class HittableList : public Hittable {
    std::vector<std::shared_ptr<Hittable>> list{};
    Aabb bBox {Aabb::empty};
public:
    HittableList() {}

    HittableList(std::shared_ptr<Hittable> ptr) {add(ptr);}

    // compatible with moving geometries
    void add(std::shared_ptr<Hittable> ptr) {
        list.emplace_back(ptr);
        bBox = Aabb{bBox, ptr->boundingBox()};
    }

    void clear() {list.clear();}

    std::size_t size() const {return list.size();}

    std::vector<std::shared_ptr<Hittable>>& elements() {return list;}

    bool hit(const Ray& ray, Interval tInterval, HitRecord& record) const override {
#ifdef ENABLE_BVH
        // std::clog << "BVH enabled in hittableList!";
        if (!bBox.hit(ray, tInterval)) return false;
#endif

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

    std::ostream& log(std::ostream& os) const override {
        if (size() == 0) {
            os << "[]";
        } else {
            os << "[";
            for (const auto& hittable: list) {
                os << *hittable << ", ";
            }
            os << "]";
        }
        return os;
    }
};

}

#endif