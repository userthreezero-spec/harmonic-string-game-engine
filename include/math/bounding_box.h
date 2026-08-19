#pragma once
#include "math/vec3.h"
#include <algorithm>

namespace hse {

struct BoundingBox {
    Vec3 min;
    Vec3 max;

    BoundingBox() : min(1e30f, 1e30f, 1e30f), max(-1e30f, -1e30f, -1e30f) {}
    BoundingBox(const Vec3& min, const Vec3& max) : min(min), max(max) {}

    bool intersects(const BoundingBox& other) const {
        return (min.x <= other.max.x && max.x >= other.min.x) &&
               (min.y <= other.max.y && max.y >= other.min.y) &&
               (min.z <= other.max.z && max.z >= other.min.z);
    }

    void expand(const Vec3& p) {
        min.x = std::min(min.x, p.x);
        min.y = std::min(min.y, p.y);
        min.z = std::min(min.z, p.z);
        max.x = std::max(max.x, p.x);
        max.y = std::max(max.y, p.y);
        max.z = std::max(max.z, p.z);
    }
};

} // namespace hse
