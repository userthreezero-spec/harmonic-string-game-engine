#include <cassert>
#include <iostream>
#include <memory>
#include <cmath>
#include "scene/picker.h"
#include "scene/scene.h"
#include "scene/primitive.h"
#include "scene/camera.h"

void test_mouse_to_object_interaction() {
    // 1. Setup Camera (Standard perspective)
    auto camera = std::make_shared<hse::Camera>(hse::ProjectionType::Perspective);
    camera->setPosition({0, 0, 5});
    camera->lookAt({0, 0, 0});
    camera->setAspectRatio(1.0f); // Square viewport
    camera->setFOV(45.0f);
    camera->update();

    // 2. Setup Scene with a Cube at the origin
    auto scene = std::make_shared<hse::Scene>();
    auto cube = std::make_shared<hse::Primitive>(hse::PrimitiveType::Cube);
    cube->setName("TargetCube");
    cube->setPosition({0, 0, 0});
    cube->setScale({1, 1, 1});
    cube->computeWorldMatrix();
    scene->addPrimitive(cube);

    // 3. Screen coordinates (Center of a 1000x1000 window)
    // This should hit the center of the cube (NDC 0,0)
    double mouseX = 500.0;
    double mouseY = 500.0;
    int width = 1000;
    int height = 1000;

    std::cout << "Testing picking at center (500, 500)..." << std::endl;
    hse::Ray ray = hse::Picker::screenToRay(mouseX, mouseY, *camera, width, height);
    auto hit = hse::Picker::pick(ray, *scene);

    assert(hit.hit);
    assert(hit.objectID == cube->getID());
    std::cout << "  PASS: Center hit confirmed." << std::endl;

    // 4. Offset test: Click slightly to the right
    // At distance 5, FOV 45, width at z=0 is 2 * 5 * tan(45/2) = 4.14m
    // mouseX = 600 (NDC x = 0.2) should be at x = 0.2 * 4.14 / 2 = 0.414m
    // This should still hit the 1.0m wide cube.
    mouseX = 600.0;
    std::cout << "Testing picking offset (600, 500)..." << std::endl;
    ray = hse::Picker::screenToRay(mouseX, mouseY, *camera, width, height);
    hit = hse::Picker::pick(ray, *scene);

    assert(hit.hit);
    assert(hit.objectID == cube->getID());
    std::cout << "  PASS: Offset hit confirmed." << std::endl;

    // 5. Miss test: Click far to the side
    mouseX = 900.0;
    std::cout << "Testing picking miss (900, 500)..." << std::endl;
    ray = hse::Picker::screenToRay(mouseX, mouseY, *camera, width, height);
    hit = hse::Picker::pick(ray, *scene);

    assert(!hit.hit);
    std::cout << "  PASS: Miss confirmed." << std::endl;
}

int main() {
    try {
        test_mouse_to_object_interaction();
        std::cout << "ALL INTERACTION TESTS PASSED!" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
