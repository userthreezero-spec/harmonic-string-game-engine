#include <cassert>
#include <iostream>
#include "scene/picker.h"
#include "scene/scene.h"
#include "scene/primitive.h"
#include "scene/camera.h"

void test_ray_intersect_unit_cube() {
    auto cube = std::make_shared<hse::Primitive>(hse::PrimitiveType::Cube);
    cube->setPosition({0, 0, 0});
    cube->setScale({1, 1, 1});
    cube->computeWorldMatrix();

    // Ray starting at (0,0,5) looking at origin
    hse::Ray ray({0, 0, 5}, {0, 0, -1});

    // Using hse::Picker::pick requires a Scene, but I'll call internal if I could.
    // Instead I'll just use a Scene.
    hse::Scene scene;
    scene.addPrimitive(cube);

    auto hit = hse::Picker::pick(ray, scene);
    assert(hit.hit == true);
    assert(hit.objectID == cube->getID());
    assert(std::abs(hit.distance - 4.5f) < 0.001f); // Hits at z=0.5

    std::cout << "PASS: ray intersect unit cube" << std::endl;
}

void test_ray_intersect_transformed_cube() {
    auto cube = std::make_shared<hse::Primitive>(hse::PrimitiveType::Cube);
    cube->setPosition({5, 0, 0});
    cube->setRotation({0, 45, 0}); // Rotated 45 degrees around Y
    cube->computeWorldMatrix();

    hse::Scene scene;
    scene.addPrimitive(cube);

    // Ray looking at the transformed cube
    hse::Ray ray({0, 0, 0}, {1, 0, 0});

    auto hit = hse::Picker::pick(ray, scene);
    assert(hit.hit == true);
    assert(hit.objectID == cube->getID());

    // The closest point of a 45-degree rotated 1x1 cube at x=5
    // corners are at +/- 0.5 in local space.
    // In world space, distance should be approx 5 - (0.5 * sqrt(2)) = 4.29
    assert(hit.distance < 5.0f);
    assert(hit.distance > 4.0f);

    std::cout << "PASS: ray intersect transformed cube" << std::endl;
}

void test_nearest_hit() {
    auto cube1 = std::make_shared<hse::Primitive>(hse::PrimitiveType::Cube);
    cube1->setName("Cube1");
    cube1->setPosition({0, 0, -5});
    cube1->computeWorldMatrix();

    auto cube2 = std::make_shared<hse::Primitive>(hse::PrimitiveType::Cube);
    cube2->setName("Cube2");
    cube2->setPosition({0, 0, -10});
    cube2->computeWorldMatrix();

    hse::Scene scene;
    scene.addPrimitive(cube1);
    scene.addPrimitive(cube2);

    hse::Ray ray({0, 0, 0}, {0, 0, -1});
    auto hit = hse::Picker::pick(ray, scene);

    assert(hit.hit == true);
    assert(hit.objectID == cube1->getID()); // Should hit the closer one

    std::cout << "PASS: nearest hit" << std::endl;
}

void test_mat4_inverse() {
    hse::Mat4 m = hse::Mat4::translate({1, 2, 3}) * hse::Mat4::rotate(30, {0, 1, 0});
    hse::Mat4 inv = m.inverse();
    hse::Mat4 res = m * inv;

    // Check diagonal
    for (int i = 0; i < 4; i++) {
        assert(std::abs(res.data[i * 5] - 1.0f) < 0.001f);
    }
    std::cout << "PASS: mat4 inverse" << std::endl;
}

int main() {
    test_mat4_inverse();
    test_ray_intersect_unit_cube();
    test_ray_intersect_transformed_cube();
    test_nearest_hit();
    std::cout << "All picking tests passed!" << std::endl;
    return 0;
}
