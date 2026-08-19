#include <cassert>
#include <iostream>
#include <cmath>
#include <memory>
#include "scene/camera.h"
#include "scene/scene.h"
#include "scene/primitive.h"

static const float PI = 3.14159265358979f;
static const float EPSILON = 0.001f;

bool approx(float a, float b) {
    return std::fabs(a - b) < EPSILON;
}

void test_orbit_disabled_by_default() {
    hse::Camera cam;
    assert(!cam.isOrbitEnabled());
    std::cout << "PASS: orbit disabled by default" << std::endl;
}

void test_orbit_enable_disable() {
    hse::Camera cam;
    cam.enableOrbit(true);
    assert(cam.isOrbitEnabled());
    cam.enableOrbit(false);
    assert(!cam.isOrbitEnabled());
    std::cout << "PASS: orbit enable/disable" << std::endl;
}

void test_orbit_radius_default() {
    hse::Camera cam;
    assert(approx(cam.getOrbitRadius(), 5.0f));
    std::cout << "PASS: orbit radius default" << std::endl;
}

void test_orbit_speed_default() {
    hse::Camera cam;
    assert(approx(cam.getOrbitSpeed(), 1.0f));
    std::cout << "PASS: orbit speed default" << std::endl;
}

void test_orbit_set_radius() {
    hse::Camera cam;
    cam.setOrbitRadius(10.0f);
    assert(approx(cam.getOrbitRadius(), 10.0f));
    std::cout << "PASS: orbit set radius" << std::endl;
}

void test_orbit_set_speed() {
    hse::Camera cam;
    cam.setOrbitSpeed(2.5f);
    assert(approx(cam.getOrbitSpeed(), 2.5f));
    std::cout << "PASS: orbit set speed" << std::endl;
}

void test_orbit_position_computation() {
    hse::Camera cam;
    cam.setOrbitRadius(5.0f);
    cam.setOrbitSpeed(0.0f);  // No speed for static test
    cam.enableOrbit(true);
    cam.updateOrbit(0.0f);

    // At yaw=0: x = target.x + radius*cos(0) = 0 + 5*1 = 5, z = target.z + radius*sin(0) = 0
    assert(approx(cam.getPosition().x, 5.0f));
    assert(approx(cam.getPosition().y, 0.0f));
    assert(approx(cam.getPosition().z, 0.0f));
    std::cout << "PASS: orbit position computation" << std::endl;
}

void test_orbit_yaw_advances() {
    hse::Camera cam;
    cam.setOrbitRadius(5.0f);
    cam.setOrbitSpeed(PI);  // π rad/s
    cam.enableOrbit(true);

    cam.updateOrbit(1.0f);  // 1 second → yaw = π

    // At yaw=π: x = 5*cos(π) = -5, z = 5*sin(π) ≈ 0
    assert(approx(cam.getPosition().x, -5.0f));
    assert(approx(cam.getPosition().z, 0.0f));
    std::cout << "PASS: orbit yaw advances" << std::endl;
}

void test_orbit_multiple_steps() {
    hse::Camera cam;
    cam.setOrbitRadius(1.0f);
    cam.setOrbitSpeed(PI / 2.0f);  // π/2 rad/s
    cam.enableOrbit(true);

    cam.updateOrbit(1.0f);  // yaw = π/2
    // x = cos(π/2) ≈ 0, z = sin(π/2) = 1
    assert(approx(cam.getPosition().x, 0.0f));
    assert(approx(cam.getPosition().z, 1.0f));

    cam.updateOrbit(1.0f);  // yaw = π
    // x = cos(π) = -1, z = sin(π) ≈ 0
    assert(approx(cam.getPosition().x, -1.0f));
    assert(approx(cam.getPosition().z, 0.0f));
    std::cout << "PASS: orbit multiple steps" << std::endl;
}

void test_orbit_disabled_no_change() {
    hse::Camera cam;
    cam.setPosition({1.0f, 2.0f, 3.0f});
    cam.enableOrbit(false);
    cam.updateOrbit(1.0f);
    assert(approx(cam.getPosition().x, 1.0f));
    assert(approx(cam.getPosition().y, 2.0f));
    assert(approx(cam.getPosition().z, 3.0f));
    std::cout << "PASS: orbit disabled no change" << std::endl;
}

void test_orbit_preserves_target() {
    hse::Camera cam;
    cam.lookAt({2.0f, 3.0f, 4.0f});
    cam.setOrbitRadius(5.0f);
    cam.setOrbitSpeed(1.0f);
    cam.enableOrbit(true);
    cam.updateOrbit(0.5f);

    // Target should remain unchanged
    assert(approx(cam.getTarget().x, 2.0f));
    assert(approx(cam.getTarget().y, 3.0f));
    assert(approx(cam.getTarget().z, 4.0f));
    std::cout << "PASS: orbit preserves target" << std::endl;
}

void test_scene_update_triggers_orbit() {
    auto scene = std::make_shared<hse::Scene>("Test");
    auto cam = std::make_shared<hse::Camera>();
    cam->setOrbitRadius(5.0f);
    cam->setOrbitSpeed(1.0f);
    cam->enableOrbit(true);
    scene->addCamera(cam);

    hse::Vec3 posBefore = cam->getPosition();
    scene->update(1.0f);
    hse::Vec3 posAfter = cam->getPosition();

    assert(!approx(posBefore.x, posAfter.x) || !approx(posBefore.z, posAfter.z));
    std::cout << "PASS: scene update triggers orbit" << std::endl;
}

int main() {
    test_orbit_disabled_by_default();
    test_orbit_enable_disable();
    test_orbit_radius_default();
    test_orbit_speed_default();
    test_orbit_set_radius();
    test_orbit_set_speed();
    test_orbit_position_computation();
    test_orbit_yaw_advances();
    test_orbit_multiple_steps();
    test_orbit_disabled_no_change();
    test_orbit_preserves_target();
    test_scene_update_triggers_orbit();
    std::cout << "All orbit tests passed!" << std::endl;
    return 0;
}
