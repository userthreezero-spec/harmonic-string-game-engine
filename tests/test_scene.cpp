#include <cassert>
#include <iostream>
#include <memory>
#include "scene/scene.h"
#include "scene/camera.h"
#include "scene/primitive.h"

void test_scene_creation() {
    hse::Scene scene("Test Scene");
    assert(scene.getName() == "Test Scene");
    assert(scene.getPrimitives().empty());
    std::cout << "PASS: scene creation" << std::endl;
}

void test_scene_add_primitive() {
    auto scene = std::make_shared<hse::Scene>("Test");
    auto prim = std::make_shared<hse::Primitive>(hse::PrimitiveType::Triangle);
    scene->addPrimitive(prim);
    assert(scene->getPrimitives().size() == 1);
    std::cout << "PASS: scene add primitive" << std::endl;
}

void test_scene_camera() {
    auto scene = std::make_shared<hse::Scene>("Test");
    auto cam = std::make_shared<hse::Camera>();
    scene->addCamera(cam);
    assert(scene->getActiveCamera() != nullptr);
    std::cout << "PASS: scene camera" << std::endl;
}

void test_camera_perspective() {
    hse::Camera cam(hse::ProjectionType::Perspective);
    cam.setFOV(60.0f);
    cam.setAspectRatio(16.0f / 9.0f);
    cam.update();
    const auto& proj = cam.getProjectionMatrix();
    assert(proj.data[0] != 0.0f);
    std::cout << "PASS: camera perspective" << std::endl;
}

void test_camera_position() {
    hse::Camera cam;
    cam.setPosition({1.0f, 2.0f, 3.0f});
    assert(cam.getPosition().x == 1.0f);
    assert(cam.getPosition().y == 2.0f);
    assert(cam.getPosition().z == 3.0f);
    std::cout << "PASS: camera position" << std::endl;
}

void test_primitive_triangle() {
    hse::Primitive prim(hse::PrimitiveType::Triangle);
    assert(prim.getVertices().size() == 9);
    assert(prim.getIndices().size() == 3);
    std::cout << "PASS: primitive triangle" << std::endl;
}

void test_primitive_position() {
    hse::Primitive prim;
    prim.setPosition({1.0f, 2.0f, 3.0f});
    assert(prim.getPosition().x == 1.0f);
    std::cout << "PASS: primitive position" << std::endl;
}

int main() {
    test_scene_creation();
    test_scene_add_primitive();
    test_scene_camera();
    test_camera_perspective();
    test_camera_position();
    test_primitive_triangle();
    test_primitive_position();
    std::cout << "All scene tests passed!" << std::endl;
    return 0;
}
