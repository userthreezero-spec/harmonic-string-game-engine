#include "math/mat4.h"
#include "math/vec3.h"
#include <cmath>
#include <cstring>

namespace hse {

Mat4::Mat4() {
    std::memset(data, 0, sizeof(data));
}

Mat4 Mat4::identity() {
    Mat4 m;
    m.data[0] = 1.0f; m.data[5] = 1.0f; m.data[10] = 1.0f; m.data[15] = 1.0f;
    return m;
}

float Mat4::toRadians(float deg) { return deg * 3.14159265358979f / 180.0f; }

Mat4 Mat4::perspective(float fovDeg, float aspect, float near, float far) {
    Mat4 m;
    float tanHalf = std::tan(toRadians(fovDeg) / 2.0f);
    m.data[0] = 1.0f / (aspect * tanHalf);
    m.data[5] = 1.0f / tanHalf;
    m.data[10] = -(far + near) / (far - near);
    m.data[11] = -1.0f;
    m.data[14] = -(2.0f * far * near) / (far - near);
    return m;
}

Mat4 Mat4::ortho(float left, float right, float bottom, float top, float near, float far) {
    Mat4 m;
    m.data[0] = 2.0f / (right - left);
    m.data[5] = 2.0f / (top - bottom);
    m.data[10] = -2.0f / (far - near);
    m.data[12] = -(right + left) / (right - left);
    m.data[13] = -(top + bottom) / (top - bottom);
    m.data[14] = -(far + near) / (far - near);
    m.data[15] = 1.0f;
    return m;
}

Mat4 Mat4::lookAt(const Vec3& eye, const Vec3& center, const Vec3& up) {
    Vec3 f = (center - eye).normalized();
    Vec3 s = f.cross(up).normalized();
    Vec3 u = s.cross(f);

    Mat4 m = identity();
    m.data[0] = s.x;  m.data[4] = s.y;  m.data[8]  = s.z;
    m.data[1] = u.x;  m.data[5] = u.y;  m.data[9]  = u.z;
    m.data[2] = -f.x; m.data[6] = -f.y; m.data[10] = -f.z;
    m.data[12] = -s.dot(eye);
    m.data[13] = -u.dot(eye);
    m.data[14] = f.dot(eye);
    return m;
}

Mat4 Mat4::translate(const Vec3& v) {
    Mat4 m = identity();
    m.data[12] = v.x; m.data[13] = v.y; m.data[14] = v.z;
    return m;
}

Mat4 Mat4::rotate(float angleDeg, const Vec3& axis) {
    Vec3 a = axis.normalized();
    float r = toRadians(angleDeg);
    float c = std::cos(r), s = std::sin(r);
    float t = 1.0f - c;

    Mat4 m = identity();
    m.data[0] = t*a.x*a.x + c;        m.data[4] = t*a.x*a.y - s*a.z;  m.data[8]  = t*a.x*a.z + s*a.y;
    m.data[1] = t*a.x*a.y + s*a.z;    m.data[5] = t*a.y*a.y + c;      m.data[9]  = t*a.y*a.z - s*a.x;
    m.data[2] = t*a.x*a.z - s*a.y;    m.data[6] = t*a.y*a.z + s*a.x;  m.data[10] = t*a.z*a.z + c;
    return m;
}

Mat4 Mat4::scale(const Vec3& v) {
    Mat4 m = identity();
    m.data[0] = v.x; m.data[5] = v.y; m.data[10] = v.z;
    return m;
}

Mat4 Mat4::operator*(const Mat4& other) const {
    Mat4 result;
    for (int col = 0; col < 4; col++) {
        for (int row = 0; row < 4; row++) {
            float sum = 0.0f;
            for (int k = 0; k < 4; k++) {
                sum += data[k * 4 + row] * other.data[col * 4 + k];
            }
            result.data[col * 4 + row] = sum;
        }
    }
    return result;
}

Vec3 Mat4::operator*(const Vec3& v) const {
    float w = data[3]*v.x + data[7]*v.y + data[11]*v.z + data[15];
    return {
        (data[0]*v.x + data[4]*v.y + data[8]*v.z  + data[12]) / w,
        (data[1]*v.x + data[5]*v.y + data[9]*v.z  + data[13]) / w,
        (data[2]*v.x + data[6]*v.y + data[10]*v.z + data[14]) / w
    };
}

Mat4 Mat4::inverse() const {
    float inv[16];
    float det;
    const float* m = data;

    inv[0] = m[5]  * m[10] * m[15] -
             m[5]  * m[11] * m[14] -
             m[9]  * m[6]  * m[15] +
             m[9]  * m[7]  * m[14] +
             m[13] * m[6]  * m[11] -
             m[13] * m[7]  * m[10];

    inv[4] = -m[4]  * m[10] * m[15] +
              m[4]  * m[11] * m[14] +
              m[8]  * m[6]  * m[15] -
              m[8]  * m[7]  * m[14] -
              m[12] * m[6]  * m[11] +
              m[12] * m[7]  * m[10];

    inv[8] = m[4]  * m[9] * m[15] -
             m[4]  * m[11] * m[13] -
             m[8]  * m[5] * m[15] +
             m[8]  * m[7] * m[13] +
             m[12] * m[5] * m[11] -
             m[12] * m[7] * m[9];

    inv[12] = -m[4]  * m[9] * m[14] +
               m[4]  * m[10] * m[13] +
               m[8]  * m[5] * m[14] -
               m[8]  * m[6] * m[13] -
               m[12] * m[5] * m[10] +
               m[12] * m[6] * m[9];

    inv[1] = -m[1]  * m[10] * m[15] +
              m[1]  * m[11] * m[14] +
              m[9]  * m[2] * m[15] -
              m[9]  * m[3] * m[14] -
              m[13] * m[2] * m[11] +
              m[13] * m[3] * m[10];

    inv[5] = m[0]  * m[10] * m[15] -
             m[0]  * m[11] * m[14] -
             m[8]  * m[2] * m[15] +
             m[8]  * m[3] * m[14] +
             m[12] * m[2] * m[11] -
             m[12] * m[3] * m[10];

    inv[9] = -m[0]  * m[9] * m[15] +
              m[0]  * m[11] * m[13] +
              m[8]  * m[1] * m[15] -
              m[8]  * m[3] * m[13] -
              m[12] * m[1] * m[11] +
              m[12] * m[3] * m[9];

    inv[13] = m[0]  * m[9] * m[14] -
              m[0]  * m[10] * m[13] -
              m[8]  * m[1] * m[14] +
              m[8]  * m[2] * m[13] +
              m[12] * m[1] * m[10] -
              m[12] * m[2] * m[9];

    inv[2] = m[1]  * m[6] * m[15] -
             m[1]  * m[7] * m[14] -
             m[5]  * m[2] * m[15] +
             m[5]  * m[3] * m[14] +
             m[13] * m[2] * m[7] -
             m[13] * m[3] * m[6];

    inv[6] = -m[0]  * m[6] * m[15] +
              m[0]  * m[7] * m[14] +
              m[4]  * m[2] * m[15] -
              m[4]  * m[3] * m[14] -
              m[12] * m[2] * m[7] +
              m[12] * m[3] * m[6];

    inv[10] = m[0]  * m[5] * m[15] -
              m[0]  * m[7] * m[13] -
              m[4]  * m[1] * m[15] +
              m[4]  * m[3] * m[13] +
              m[12] * m[1] * m[7] -
              m[12] * m[3] * m[5];

    inv[14] = -m[0]  * m[5] * m[14] +
               m[0]  * m[6] * m[13] +
               m[4]  * m[1] * m[14] -
               m[4]  * m[2] * m[13] -
               m[12] * m[1] * m[6] +
               m[12] * m[2] * m[5];

    inv[3] = -m[1] * m[6] * m[11] +
              m[1] * m[7] * m[10] +
              m[5] * m[2] * m[11] -
              m[5] * m[3] * m[10] -
              m[9] * m[2] * m[7] +
              m[9] * m[3] * m[6];

    inv[7] = m[0] * m[6] * m[11] -
             m[0] * m[7] * m[10] -
             m[4] * m[2] * m[11] +
             m[4] * m[3] * m[10] +
             m[8] * m[2] * m[7] -
             m[8] * m[3] * m[6];

    inv[11] = -m[0] * m[5] * m[11] +
               m[0] * m[7] * m[9] +
               m[4] * m[1] * m[11] -
               m[4] * m[3] * m[9] -
               m[8] * m[1] * m[7] +
               m[8] * m[3] * m[5];

    inv[15] = m[0] * m[5] * m[10] -
              m[0] * m[6] * m[9] -
              m[4] * m[1] * m[10] +
              m[4] * m[2] * m[9] +
              m[8] * m[1] * m[6] -
              m[8] * m[2] * m[5];

    det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

    if (det == 0) return Mat4::identity();

    det = 1.0f / det;

    Mat4 res;
    for (int i = 0; i < 16; i++) res.data[i] = inv[i] * det;
    return res;
}

} // namespace hse
