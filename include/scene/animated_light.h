#pragma once
#include "math/vec3.h"
#include "scene/light.h"
#include "scene/animation.h"
#include "scene/timeline.h"
#include <cmath>

namespace hse {

struct AnimatedLight {
    Light* light = nullptr;
    Vec3 basePosition{0.0f, 0.0f, 0.0f};
    float orbitRadius = 0.0f;
    float orbitSpeed = 0.0f;
    float orbitAngle = 0.0f;
    bool colorAnimate = false;
    Vec3 colorA{1.0f, 1.0f, 1.0f};
    Vec3 colorB{1.0f, 1.0f, 1.0f};
    float colorSpeed = 1.0f;
    Timeline* timeline = nullptr;

    void update(float deltaTime, float time) {
        if (!light) return;

        if (timeline) {
            if (timeline->hasPositionKeyframes()) {
                light->position = timeline->evaluatePosition();
            }
            if (timeline->hasColorKeyframes()) {
                light->color = timeline->evaluateColor();
            }
            return;
        }

        if (orbitRadius > 0.0f && orbitSpeed != 0.0f) {
            orbitAngle += orbitSpeed * deltaTime;
            light->position.x = basePosition.x + std::cos(orbitAngle) * orbitRadius;
            light->position.y = basePosition.y;
            light->position.z = basePosition.z + std::sin(orbitAngle) * orbitRadius;
        }

        if (colorAnimate) {
            float t = (std::sin(time * colorSpeed) + 1.0f) * 0.5f;
            light->color = Animation::lerp(colorA, colorB, t);
        }
    }
};

} // namespace hse
