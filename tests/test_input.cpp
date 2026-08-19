#include <cassert>
#include <iostream>
#include <cmath>
#include "scene/camera.h"
#include "math/vec3.h"

void test_camera_manual_orbit() {
    hse::Camera cam;
    cam.setOrbitRadius(10.0f);
    cam.lookAt({0, 0, 0});

    // Initial pos: (10, 0, 0) assuming yaw=0, pitch=0
    cam.updateOrbit(0.0f);
    assert(std::abs(cam.getPosition().x - 10.0f) < 0.001f);
    assert(std::abs(cam.getPosition().y - 0.0f) < 0.001f);
    assert(std::abs(cam.getPosition().z - 0.0f) < 0.001f);

    // Offset orbit by 90 degrees yaw
    cam.offsetOrbit(90.0f, 0.0f);
    // cos(90) = 0, sin(90) = 1 -> (0, 0, 10)
    assert(std::abs(cam.getPosition().x - 0.0f) < 0.001f);
    assert(std::abs(cam.getPosition().z - 10.0f) < 0.001f);

    // Offset orbit by 45 degrees pitch (not clamped)
    cam.offsetOrbit(0.0f, 45.0f);
    // yaw=90, pitch=45 -> x=0, y=10*sin(45), z=10*cos(45)
    float expected = 10.0f * std::sin(45.0f * 3.14159265f / 180.0f);
    assert(std::abs(cam.getPosition().y - expected) < 0.001f);
    assert(std::abs(cam.getPosition().z - expected) < 0.001f);

    std::cout << "PASS: camera manual orbit" << std::endl;
}

void test_camera_zoom() {
    hse::Camera cam;
    cam.setOrbitRadius(10.0f);
    cam.updateOrbit(0.0f);

    cam.zoom(2.0f); // radius 10 -> 8
    assert(cam.getOrbitRadius() == 8.0f);

    cam.zoom(10.0f); // radius 8 -> 0.1 (clamped)
    assert(cam.getOrbitRadius() == 0.1f);

    std::cout << "PASS: camera zoom" << std::endl;
}

void test_camera_pan() {
    hse::Camera cam;
    cam.setPosition({0, 0, 10});
    cam.lookAt({0, 0, 0});
    cam.update();

    // Forward is (0,0,-1), Right is (1,0,0), Up is (0,1,0)
    cam.pan(2.0f, 3.0f);

    // Target should move to (2, 3, 0)
    assert(cam.getTarget().x == 2.0f);
    assert(cam.getTarget().y == 3.0f);
    assert(cam.getTarget().z == 0.0f);

    // Position should move to (2, 3, 10)
    assert(cam.getPosition().x == 2.0f);
    assert(cam.getPosition().y == 3.0f);
    assert(cam.getPosition().z == 10.0f);

    std::cout << "PASS: camera pan" << std::endl;
}

int main() {
    test_camera_manual_orbit();
    test_camera_zoom();
    test_camera_pan();
    std::cout << "All input tests passed!" << std::endl;
    return 0;
}
