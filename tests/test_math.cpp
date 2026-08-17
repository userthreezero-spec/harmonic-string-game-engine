#include <cassert>
#include <iostream>
#include <cmath>
#include "math/vec3.h"
#include "math/mat4.h"

static bool approxEqual(float a, float b, float eps = 0.001f) {
    return std::abs(a - b) < eps;
}

void test_vec3_basic_ops() {
    hse::Vec3 a(1.0f, 2.0f, 3.0f);
    hse::Vec3 b(4.0f, 5.0f, 6.0f);

    auto c = a + b;
    assert(approxEqual(c.x, 5.0f) && approxEqual(c.y, 7.0f) && approxEqual(c.z, 9.0f));

    auto d = b - a;
    assert(approxEqual(d.x, 3.0f) && approxEqual(d.y, 3.0f) && approxEqual(d.z, 3.0f));

    auto e = a * 2.0f;
    assert(approxEqual(e.x, 2.0f) && approxEqual(e.y, 4.0f) && approxEqual(e.z, 6.0f));

    std::cout << "PASS: vec3 basic operations" << std::endl;
}

void test_vec3_length() {
    hse::Vec3 v(3.0f, 4.0f, 0.0f);
    assert(approxEqual(v.length(), 5.0f));
    std::cout << "PASS: vec3 length" << std::endl;
}

void test_vec3_normalize() {
    hse::Vec3 v(3.0f, 4.0f, 0.0f);
    auto n = v.normalized();
    assert(approxEqual(n.length(), 1.0f));
    std::cout << "PASS: vec3 normalize" << std::endl;
}

void test_vec3_dot() {
    hse::Vec3 a(1.0f, 0.0f, 0.0f);
    hse::Vec3 b(0.0f, 1.0f, 0.0f);
    assert(approxEqual(a.dot(b), 0.0f));
    std::cout << "PASS: vec3 dot product" << std::endl;
}

void test_vec3_cross() {
    hse::Vec3 x(1.0f, 0.0f, 0.0f);
    hse::Vec3 y(0.0f, 1.0f, 0.0f);
    auto z = x.cross(y);
    assert(approxEqual(z.x, 0.0f) && approxEqual(z.y, 0.0f) && approxEqual(z.z, 1.0f));
    std::cout << "PASS: vec3 cross product" << std::endl;
}

void test_mat4_identity() {
    auto m = hse::Mat4::identity();
    assert(approxEqual(m.data[0], 1.0f) && approxEqual(m.data[5], 1.0f) && approxEqual(m.data[10], 1.0f) && approxEqual(m.data[15], 1.0f));
    assert(approxEqual(m.data[1], 0.0f) && approxEqual(m.data[4], 0.0f));
    std::cout << "PASS: mat4 identity" << std::endl;
}

void test_mat4_translate() {
    auto m = hse::Mat4::translate({1.0f, 2.0f, 3.0f});
    assert(approxEqual(m.data[12], 1.0f) && approxEqual(m.data[13], 2.0f) && approxEqual(m.data[14], 3.0f));
    std::cout << "PASS: mat4 translate" << std::endl;
}

void test_mat4_multiply_identity() {
    auto a = hse::Mat4::identity();
    auto b = hse::Mat4::translate({1.0f, 2.0f, 3.0f});
    auto c = a * b;
    assert(approxEqual(c.data[12], 1.0f) && approxEqual(c.data[13], 2.0f) && approxEqual(c.data[14], 3.0f));
    std::cout << "PASS: mat4 multiply identity" << std::endl;
}

void test_mat4_perspective() {
    auto m = hse::Mat4::perspective(45.0f, 16.0f/9.0f, 0.1f, 1000.0f);
    assert(!approxEqual(m.data[0], 0.0f));
    assert(!approxEqual(m.data[5], 0.0f));
    assert(approxEqual(m.data[11], -1.0f));
    std::cout << "PASS: mat4 perspective" << std::endl;
}

void test_mat4_lookat() {
    auto m = hse::Mat4::lookAt({0,0,5}, {0,0,0}, {0,1,0});
    assert(!approxEqual(m.data[10], 0.0f));
    std::cout << "PASS: mat4 lookAt" << std::endl;
}

void test_mat4_rotate() {
    auto m = hse::Mat4::rotate(90.0f, {0,1,0});
    assert(approxEqual(m.data[0], 0.0f, 0.01f));
    assert(approxEqual(m.data[10], 0.0f, 0.01f));
    std::cout << "PASS: mat4 rotate" << std::endl;
}

void test_mat4_scale() {
    auto m = hse::Mat4::scale({2.0f, 3.0f, 4.0f});
    assert(approxEqual(m.data[0], 2.0f));
    assert(approxEqual(m.data[5], 3.0f));
    assert(approxEqual(m.data[10], 4.0f));
    std::cout << "PASS: mat4 scale" << std::endl;
}

int main() {
    test_vec3_basic_ops();
    test_vec3_length();
    test_vec3_normalize();
    test_vec3_dot();
    test_vec3_cross();
    test_mat4_identity();
    test_mat4_translate();
    test_mat4_multiply_identity();
    test_mat4_perspective();
    test_mat4_lookat();
    test_mat4_rotate();
    test_mat4_scale();
    std::cout << "All math tests passed!" << std::endl;
    return 0;
}
