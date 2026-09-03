#pragma once
#include "math/vec3.h"

namespace hse {

struct Animation {
    static float lerp(float a, float b, float t) {
        return a + (b - a) * clamp01(t);
    }

    static Vec3 lerp(const Vec3& a, const Vec3& b, float t) {
        float ct = clamp01(t);
        return {
            a.x + (b.x - a.x) * ct,
            a.y + (b.y - a.y) * ct,
            a.z + (b.z - a.z) * ct
        };
    }

    static float clamp01(float t) {
        if (t < 0.0f) return 0.0f;
        if (t > 1.0f) return 1.0f;
        return t;
    }
};

} // namespace hse
