#include <cassert>
#include <iostream>
#include <memory>
#include "scene/scene.h"
#include "scene/camera.h"
#include "scene/primitive.h"
#include "scene/scene_builder.h"
#include "scene/light.h"

int main() {
    hse::ProjectManifest manifest;
    auto scene = hse::SceneBuilder::importState("C:/Users/Rico/HarmonicStringEngine/experiments/hse019/room.hsc", manifest);

    assert(scene != nullptr);
    assert(manifest.projectName.find("HSE-019 Room") != std::string::npos);

    // Check primitives
    // floor, ceiling, wall_back, wall_left, wall_right,
    // wall_front_left, wall_front_right, wall_front_top,
    // window_frame,
    // table_top, table_leg1-4,
    // chair_1, chair_1_back, chair_2, chair_2_back
    // Total should be 23 after hierarchical reconstruction
    std::cout << "Primitive count: " << scene->getPrimitiveCount() << std::endl;
    assert(scene->getPrimitiveCount() == 23);

    // Check lights
    std::cout << "Light count: " << scene->getLightCount() << std::endl;
    assert(scene->getLightCount() == 1);
    auto light = scene->getLights()[0];
    assert(light.getIntensity() == 1.5f);

    // Check camera
    assert(scene->getCameraCount() == 1);
    auto camera = scene->getActiveCamera();
    assert(camera->getPosition().x == 4.5f);

    // Check that object exists
    auto chair = scene->findByName("chair_1");
    assert(chair != nullptr);

    std::cout << "PASS: room.hsc persistence verification" << std::endl;
    return 0;
}
