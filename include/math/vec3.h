#pragma once

namespace hse {

struct Vec3 {
    float x, y, z;

    Vec3(float x = 0.0f, float y = 0.0f, float z = 0.0f) : x(x), y(y), z(z) {}

    Vec3 operator+(const Vec3& other) const;
    Vec3 operator-(const Vec3& other) const;
    Vec3 operator*(float scalar) const;
    Vec3& operator+=(const Vec3& other);

    float length() const;
    Vec3 normalized() const;
    float dot(const Vec3& other) const;
    Vec3 cross(const Vec3& other) const;
};

} // namespace hse
