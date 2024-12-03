#ifndef GEO_HITTABLE_H
#define GEO_HITTABLE_H

#include "../mixture/record.h"
#include "aabb.h"

#include <vector>
#include <memory>

namespace krt {

class Hittable {
public:
    virtual ~Hittable() = default;

    virtual bool hit(const Ray& ray, Interval tInterval, HitRecord& record) const = 0;

    virtual const Aabb& boundingBox() const = 0;

    virtual double pdfVal(const Point& origin, const Vec3& dir, double time) const {
        return 0.0;
    }

    virtual Vec3 random(const Point& p, double time) const {
        return Vec3 {1, 0, 0};
    }
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

    double pdfVal(const Point& origin, const Vec3& dir, double time) const override {
        if (size() == 0) return 0;
        double weight {1.0 / size()};
        double res {0.0};
        for (const std::shared_ptr<Hittable>& obj: list) {
            res += weight * obj->pdfVal(origin, dir, time);
        }
        return res;
    }

    Vec3 random(const Point& p, double time) const override {
        if (size() == 0) return Vec3{1, 0, 0};
        // randomly choose one
        return list[randomInt(0, size())]->random(p, time);
    }
};

class HittablePdf : public Pdf {
    const Hittable& obj;
    const Point& origin;
public:
    HittablePdf(const Hittable& obj, const Point& origin) : obj {obj}, origin {origin} {

    }

    double value(const Vec3& dir, double time) const override {
        return obj.pdfVal(origin, dir, time);
    }

    Vec3 generate(double time) const override {
        return obj.random(origin, time);
    }
};

}

#endif