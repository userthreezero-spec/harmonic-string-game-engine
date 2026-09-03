#pragma once
#include <vector>
#include "scene/keyframe.h"
#include "scene/animation.h"
#include "math/vec3.h"

namespace hse {

enum class LoopMode { Once, Loop, PingPong };

class Timeline {
public:
    Timeline() = default;

    void addPositionKeyframe(float time, const Vec3& value);
    void addRotationKeyframe(float time, const Vec3& value);
    void addScaleKeyframe(float time, const Vec3& value);
    void addColorKeyframe(float time, const Vec3& value);

    void play();
    void pause();
    void stop();
    void setLoopMode(LoopMode mode);
    void setSpeed(float speed);
    void setDuration(float duration);

    void update(float deltaTime);

    Vec3 evaluatePosition() const;
    Vec3 evaluateRotation() const;
    Vec3 evaluateScale() const;
    Vec3 evaluateColor() const;

    float getTime() const { return m_time; }
    float getDuration() const { return m_duration; }
    bool isPlaying() const { return m_playing; }
    LoopMode getLoopMode() const { return m_loopMode; }
    float getSpeed() const { return m_speed; }

    bool hasPositionKeyframes() const { return !m_positionKeyframes.empty(); }
    bool hasRotationKeyframes() const { return !m_rotationKeyframes.empty(); }
    bool hasScaleKeyframes() const { return !m_scaleKeyframes.empty(); }
    bool hasColorKeyframes() const { return !m_colorKeyframes.empty(); }

    size_t getPositionKeyframeCount() const { return m_positionKeyframes.size(); }
    size_t getRotationKeyframeCount() const { return m_rotationKeyframes.size(); }
    size_t getScaleKeyframeCount() const { return m_scaleKeyframes.size(); }
    size_t getColorKeyframeCount() const { return m_colorKeyframes.size(); }

private:
    std::vector<Keyframe<Vec3>> m_positionKeyframes;
    std::vector<Keyframe<Vec3>> m_rotationKeyframes;
    std::vector<Keyframe<Vec3>> m_scaleKeyframes;
    std::vector<Keyframe<Vec3>> m_colorKeyframes;

    float m_duration = 1.0f;
    float m_time = 0.0f;
    bool m_playing = false;
    LoopMode m_loopMode = LoopMode::Once;
    float m_speed = 1.0f;

    static Vec3 evaluateKeyframes(const std::vector<Keyframe<Vec3>>& keyframes, float time);
    static void sortKeyframes(std::vector<Keyframe<Vec3>>& keyframes);
};

} // namespace hse
