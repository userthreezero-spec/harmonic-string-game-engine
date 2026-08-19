#pragma once
#include "math/vec3.h"
#include <cstdint>

namespace hse {

struct Ray {
    Vec3 origin;
    Vec3 direction;

    Ray(const Vec3& origin = Vec3(), const Vec3& direction = Vec3(0, 0, -1))
        : origin(origin), direction(direction.normalized()) {}
};

struct Intersection {
    bool hit = false;
    float distance = 1e30f;
    Vec3 point;
    uint64_t objectID = 0;

    operator bool() const { return hit; }
};

} // namespace hse
