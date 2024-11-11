#ifndef GEO_BVH_H
#define GEO_BVH_H

#include "hittable.h"

namespace krt {

// bounding volume hierarchy
class BvhNode : public Hittable {
public:
    BvhNode(HittableList& list) : BvhNode(list.elements(), 0, list.size()) {}

    BvhNode(std::vector<std::shared_ptr<Hittable>>& objects, size_t start, size_t end) {
        for (const auto& obj: objects) {
            bBox = Aabb{bBox, obj->boundingBox()};
        }
        // find the longest axis to divide
        int axis = bBox.longestAxis();
        size_t numOfObj = end - start;
        if (numOfObj == 1) {
            left = right = objects[start];
        } else if (numOfObj == 2) {
            left = objects[start];
            right = objects[start + 1];
        } else {
            // sort on axis
            auto cmp = axis == 0 ? cmpOnX : axis == 1 ? cmpOnY : cmpOnZ;
            std::sort(objects.begin() + start, objects.begin() + end, cmp);
            size_t mid = start + numOfObj / 2;
            left = std::make_shared<BvhNode>(objects, start, mid);
            right = std::make_shared<BvhNode>(objects, mid, end);
        }
    }

    bool hit(const Ray& ray, Interval tInterval, HitRecord& record) const override;

    const Aabb& boundingBox() const override {
        return bBox;
    }

private:
    std::shared_ptr<Hittable> left;
    std::shared_ptr<Hittable> right;
    Aabb bBox{};

    static bool cmpOnAxis(const std::shared_ptr<Hittable> o1, const std::shared_ptr<Hittable> o2, int axis) {
        return o1->boundingBox()[axis].min < o2->boundingBox()[axis].min;
    }

    static bool cmpOnX(const std::shared_ptr<Hittable> o1, const std::shared_ptr<Hittable> o2) {
        return cmpOnAxis(o1, o2, 0);
    }

    static bool cmpOnY(const std::shared_ptr<Hittable> o1, const std::shared_ptr<Hittable> o2) {
        return cmpOnAxis(o1, o2, 1);
    }

    static bool cmpOnZ(const std::shared_ptr<Hittable> o1, const std::shared_ptr<Hittable> o2) {
        return cmpOnAxis(o1, o2, 2);
    }
};

bool BvhNode::hit(const Ray& ray, Interval tInterval, HitRecord& record) const {
    if (!bBox.hit(ray, tInterval)) return false;
    bool hitLeft = left->hit(ray, tInterval, record);
    bool hitRight = right->hit(ray, Interval{tInterval.min, hitLeft ? record.t : tInterval.max}, record);
    return hitLeft || hitRight;
}

}

#endif