#pragma once
#include "scene/timeline.h"
#include "math/vec3.h"

namespace hse {

struct AnimatedProperty {
    Timeline* timeline = nullptr;
    Vec3* targetPosition = nullptr;
    Vec3* targetRotation = nullptr;
    Vec3* targetScale = nullptr;
    Vec3* targetColor = nullptr;

    void update() {
        if (!timeline) return;

        if (targetPosition && timeline->hasPositionKeyframes()) {
            *targetPosition = timeline->evaluatePosition();
        }
        if (targetRotation && timeline->hasRotationKeyframes()) {
            *targetRotation = timeline->evaluateRotation();
        }
        if (targetScale && timeline->hasScaleKeyframes()) {
            *targetScale = timeline->evaluateScale();
        }
        if (targetColor && timeline->hasColorKeyframes()) {
            *targetColor = timeline->evaluateColor();
        }
    }
};

} // namespace hse
