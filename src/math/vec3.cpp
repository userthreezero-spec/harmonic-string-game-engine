#include "math/vec3.h"
#include <cmath>

namespace hse {

Vec3 Vec3::operator+(const Vec3& other) const { return {x + other.x, y + other.y, z + other.z}; }
Vec3 Vec3::operator-(const Vec3& other) const { return {x - other.x, y - other.y, z - other.z}; }
Vec3 Vec3::operator*(float scalar) const { return {x * scalar, y * scalar, z * scalar}; }
Vec3& Vec3::operator+=(const Vec3& other) { x += other.x; y += other.y; z += other.z; return *this; }

float Vec3::length() const { return std::sqrt(x*x + y*y + z*z); }

Vec3 Vec3::normalized() const {
    float len = length();
    if (len == 0.0f) return {0, 0, 0};
    return *this * (1.0f / len);
}

float Vec3::dot(const Vec3& other) const { return x*other.x + y*other.y + z*other.z; }

Vec3 Vec3::cross(const Vec3& other) const {
    return {
        y * other.z - z * other.y,
        z * other.x - x * other.z,
        x * other.y - y * other.x
    };
}

} // namespace hse
