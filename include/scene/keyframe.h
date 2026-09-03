#pragma once

namespace hse {

template<typename T>
struct Keyframe {
    float time;
    T value;

    Keyframe() : time(0.0f), value() {}
    Keyframe(float t, const T& v) : time(t), value(v) {}
};

} // namespace hse
