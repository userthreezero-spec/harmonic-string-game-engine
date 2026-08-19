#pragma once
#include "vec3.h"

namespace hse {

class Mat4 {
public:
    float data[16];

    Mat4();

    static Mat4 identity();
    static Mat4 perspective(float fovDeg, float aspect, float near, float far);
    static Mat4 ortho(float left, float right, float bottom, float top, float near, float far);
    static Mat4 lookAt(const Vec3& eye, const Vec3& center, const Vec3& up);
    static Mat4 translate(const Vec3& v);
    static Mat4 rotate(float angleDeg, const Vec3& axis);
    static Mat4 scale(const Vec3& v);

    Mat4 operator*(const Mat4& other) const;
    Vec3 operator*(const Vec3& v) const;

    Mat4 inverse() const;

    const float* ptr() const { return data; }

private:
    static float toRadians(float deg);
};

} // namespace hse
