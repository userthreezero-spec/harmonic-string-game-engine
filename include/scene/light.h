#pragma once
#include "math/vec3.h"
#include <string>

namespace hse {

enum class LightType { Point, Directional };

struct Light {
    LightType type = LightType::Point;
    std::string name = "Light";
    Vec3 position{0.0f, 0.0f, 0.0f};
    Vec3 direction{0.0f, -1.0f, 0.0f};
    Vec3 color{1.0f, 1.0f, 1.0f};
    float ambient = 0.1f;
    float diffuse = 0.8f;
    float specular = 1.0f;
    float intensity = 1.0f;

    Light() = default;
    Light(LightType type) : type(type) {}
    Light(const std::string& name, LightType type = LightType::Point) : type(type), name(name) {}

    void setPosition(const Vec3& p) { position = p; }
    void setColor(const Vec3& c) { color = c; }
    void setIntensity(float i) { intensity = i; }
    const Vec3& getPosition() const { return position; }
    const Vec3& getColor() const { return color; }
    float getIntensity() const { return intensity; }
    const std::string& getName() const { return name; }
};

} // namespace hse
