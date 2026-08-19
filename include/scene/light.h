#pragma once
#include "math/vec3.h"
#include <string>

namespace hse {

enum class LightType { Point, Directional, Spot };

class Light {
public:
    Light(const std::string& name = "Light")
        : m_name(name), m_type(LightType::Point), m_position(0, 0, 0), m_color(1, 1, 1), m_intensity(1.0f) {}

    const std::string& getName() const { return m_name; }
    void setName(const std::string& name) { m_name = name; }

    LightType getType() const { return m_type; }
    void setType(LightType type) { m_type = type; }

    const Vec3& getPosition() const { return m_position; }
    void setPosition(const Vec3& pos) { m_position = pos; }

    const Vec3& getColor() const { return m_color; }
    void setColor(const Vec3& color) { m_color = color; }

    float getIntensity() const { return m_intensity; }
    void setIntensity(float intensity) { m_intensity = intensity; }

private:
    std::string m_name;
    LightType m_type;
    Vec3 m_position;
    Vec3 m_color;
    float m_intensity;
};

} // namespace hse
