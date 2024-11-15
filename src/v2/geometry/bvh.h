#ifndef GEO_HBV_H
#define GEO_HBV_H

#include "hittable.h"
#include "aabb.h"

namespace krt {

// bounding volume hierarchy
class BvhNode : public Hittable {
public:
    BvhNode(HittableList& list) : BvhNode(list.elements(), 0, list.size()) {}

    BvhNode(std::vector<std::shared_ptr<Hittable>> vec, size_t iStart, size_t iEnd) {
        // build bBox
        bBox = Aabb::empty;
        if (iEnd == iStart) return;
        for (size_t i = iStart; i < iEnd; ++i) {
            bBox = Aabb(bBox, vec[i]->boundingBox());
        }
        // find the axis to divide
        size_t axis = bBox.longestAxis();
        size_t count = iEnd - iStart;
        // divide
        if (count == 1) {
            left = right = vec[iStart];
        } else if (count == 2) {
            left = vec[iStart];
            right = vec[iStart + 1];
        } else {
            auto cmp = cmpOnX;
            switch (axis)
            {
            case 1:
                cmp = cmpOnY;
                break;
            case 2:
                cmp = cmpOnZ;
                break;
            }
            std::sort(std::begin(vec) + iStart, std::begin(vec) + iEnd, cmp);
            size_t iMid = iStart + count / 2;
            left = std::make_shared<BvhNode>(vec, iStart, iMid);
            right = std::make_shared<BvhNode>(vec, iMid, iEnd);
        }
    }

    bool hit(const Ray& ray, Interval tInterval, HitRecord& record) const override {
        if (!bBox.hit(ray, tInterval)) return false;
        bool hitLeft {left && left->hit(ray, tInterval, record)};
        bool hitRight {right && right->hit(ray, Interval{tInterval.min, hitLeft ? record.t : tInterval.max}, record)};
        return hitLeft || hitRight;
    }

    const Aabb& boundingBox() const override {
        return bBox;
    }

private:
    Aabb bBox;
    std::shared_ptr<Hittable> left;
    std::shared_ptr<Hittable> right;

    static bool cmpOnAxis(size_t axis, const std::shared_ptr<Hittable>& p, const std::shared_ptr<Hittable>& q) {
        return p->boundingBox()[axis].min < q->boundingBox()[axis].min;
    }

    static bool cmpOnX(const std::shared_ptr<Hittable>& p, const std::shared_ptr<Hittable>& q) {
        return cmpOnAxis(0, p, q);
    }

    static bool cmpOnY(const std::shared_ptr<Hittable>& p, const std::shared_ptr<Hittable>& q) {
        return cmpOnAxis(1, p, q);
    }

    static bool cmpOnZ(const std::shared_ptr<Hittable>& p, const std::shared_ptr<Hittable>& q) {
        return cmpOnAxis(2, p, q);
    }
};

}

#endif
