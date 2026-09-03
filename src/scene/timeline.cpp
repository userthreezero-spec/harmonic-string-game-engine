#include "scene/timeline.h"
#include <algorithm>

namespace hse {

void Timeline::addPositionKeyframe(float time, const Vec3& value) {
    m_positionKeyframes.emplace_back(time, value);
    sortKeyframes(m_positionKeyframes);
    if (time > m_duration) m_duration = time;
}

void Timeline::addRotationKeyframe(float time, const Vec3& value) {
    m_rotationKeyframes.emplace_back(time, value);
    sortKeyframes(m_rotationKeyframes);
    if (time > m_duration) m_duration = time;
}

void Timeline::addScaleKeyframe(float time, const Vec3& value) {
    m_scaleKeyframes.emplace_back(time, value);
    sortKeyframes(m_scaleKeyframes);
    if (time > m_duration) m_duration = time;
}

void Timeline::addColorKeyframe(float time, const Vec3& value) {
    m_colorKeyframes.emplace_back(time, value);
    sortKeyframes(m_colorKeyframes);
    if (time > m_duration) m_duration = time;
}

void Timeline::play() { m_playing = true; }
void Timeline::pause() { m_playing = false; }
void Timeline::stop() { m_playing = false; m_time = 0.0f; }
void Timeline::setLoopMode(LoopMode mode) { m_loopMode = mode; }
void Timeline::setSpeed(float speed) { m_speed = speed; }
void Timeline::setDuration(float duration) { m_duration = duration; }

void Timeline::update(float deltaTime) {
    if (!m_playing) return;
    if (m_duration <= 0.0f) return;

    m_time += deltaTime * m_speed;

    switch (m_loopMode) {
        case LoopMode::Once:
            if (m_time >= m_duration) {
                m_time = m_duration;
                m_playing = false;
            }
            break;
        case LoopMode::Loop:
            while (m_time >= m_duration) {
                m_time -= m_duration;
            }
            break;
        case LoopMode::PingPong: {
            float cycle = m_duration * 2.0f;
            while (m_time >= cycle) {
                m_time -= cycle;
            }
            if (m_time > m_duration) {
                m_time = cycle - m_time;
            }
            break;
        }
    }
}

Vec3 Timeline::evaluatePosition() const {
    return evaluateKeyframes(m_positionKeyframes, m_time);
}

Vec3 Timeline::evaluateRotation() const {
    return evaluateKeyframes(m_rotationKeyframes, m_time);
}

Vec3 Timeline::evaluateScale() const {
    return evaluateKeyframes(m_scaleKeyframes, m_time);
}

Vec3 Timeline::evaluateColor() const {
    return evaluateKeyframes(m_colorKeyframes, m_time);
}

Vec3 Timeline::evaluateKeyframes(const std::vector<Keyframe<Vec3>>& keyframes, float time) {
    if (keyframes.empty()) return {0.0f, 0.0f, 0.0f};
    if (keyframes.size() == 1) return keyframes[0].value;

    if (time <= keyframes[0].time) return keyframes[0].value;
    if (time >= keyframes.back().time) return keyframes.back().value;

    for (size_t i = 0; i < keyframes.size() - 1; i++) {
        if (time >= keyframes[i].time && time <= keyframes[i + 1].time) {
            float span = keyframes[i + 1].time - keyframes[i].time;
            if (span <= 0.0f) return keyframes[i].value;
            float t = (time - keyframes[i].time) / span;
            return Animation::lerp(keyframes[i].value, keyframes[i + 1].value, t);
        }
    }

    return keyframes.back().value;
}

void Timeline::sortKeyframes(std::vector<Keyframe<Vec3>>& keyframes) {
    std::sort(keyframes.begin(), keyframes.end(),
        [](const Keyframe<Vec3>& a, const Keyframe<Vec3>& b) {
            return a.time < b.time;
        });
}

} // namespace hse
