#pragma once
#include "math/vec3.h"
#include "scene/animation.h"
#include "scene/primitive.h"
#include "scene/timeline.h"
#include <memory>
#include <cmath>

namespace hse {

struct AnimatedPrimitive {
    std::shared_ptr<Primitive> primitive;
    Vec3 basePosition{0.0f, 0.0f, 0.0f};
    float bobAmplitude = 0.0f;
    float bobSpeed = 0.0f;
    float rotationSpeed = 0.0f;
    Vec3 rotationAxis{0.0f, 1.0f, 0.0f};
    bool colorAnimate = false;
    Vec3 colorA{1.0f, 1.0f, 1.0f};
    Vec3 colorB{1.0f, 1.0f, 1.0f};
    float colorSpeed = 1.0f;
    Timeline* timeline = nullptr;

    void update(float deltaTime, float time) {
        if (!primitive) return;

        if (timeline) {
            if (timeline->hasPositionKeyframes()) {
                primitive->setPosition(timeline->evaluatePosition());
            }
            if (timeline->hasRotationKeyframes()) {
                primitive->setRotation(timeline->evaluateRotation());
            }
            if (timeline->hasScaleKeyframes()) {
                primitive->setScale(timeline->evaluateScale());
            }
            if (timeline->hasColorKeyframes()) {
                primitive->setColor(timeline->evaluateColor());
            }
            return;
        }

        Vec3 pos = basePosition;
        if (bobAmplitude > 0.0f && bobSpeed != 0.0f) {
            pos.y += std::sin(time * bobSpeed) * bobAmplitude;
        }
        primitive->setPosition(pos);

        if (rotationSpeed != 0.0f) {
            float angle = time * rotationSpeed;
            Vec3 rot = rotationAxis * angle;
            primitive->setRotation(rot);
        }

        if (colorAnimate) {
            float t = (std::sin(time * colorSpeed) + 1.0f) * 0.5f;
            primitive->setColor(Animation::lerp(colorA, colorB, t));
        }
    }
};

} // namespace hse
