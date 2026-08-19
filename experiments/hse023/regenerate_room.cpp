#include "scene/scene.h"
#include "scene/camera.h"
#include "scene/scene_builder.h"
#include <iostream>

int main() {
    auto scene = hse::SceneBuilder::buildRoom();
    auto camera = scene->getActiveCamera();
    if (!camera) {
        camera = std::make_shared<hse::Camera>();
        camera->setPosition({4.5f, 2.0f, 4.5f});
        camera->lookAt({0,0,0});
        camera->update();
    }

    hse::ProjectManifest manifest;
    manifest.projectName = "HSE-019 Room (Hierarchical)";

    std::string path = "C:/Users/Rico/HarmonicStringEngine/experiments/hse019/room.hsc";
    hse::SceneBuilder::exportHSC(*scene, *camera, manifest, path);

    std::cout << "room.hsc regenerated with assemblies." << std::endl;
    return 0;
}
