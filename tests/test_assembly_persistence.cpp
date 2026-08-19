#include <cassert>
#include <iostream>
#include <memory>
#include "scene/scene.h"
#include "scene/camera.h"
#include "scene/primitive.h"
#include "scene/scene_builder.h"

int main() {
    auto scene = hse::SceneBuilder::buildRoom();
    assert(scene->getPrimitiveCount() > 0);

    // Check if table exists as assembly
    auto table = scene->findByName("table");
    assert(table != nullptr);
    assert(table->getType() == hse::PrimitiveType::Group);
    assert(!table->getChildren().empty());

    // Save to temp file
    hse::ProjectManifest manifest;
    manifest.projectName = "AssemblyTest";
    std::string path = "temp_assembly.hsc";

    auto camera = std::make_shared<hse::Camera>();
    hse::SceneBuilder::exportHSC(*scene, *camera, manifest, path);

    // Load back
    hse::ProjectManifest manifest2;
    auto scene2 = hse::SceneBuilder::importState(path, manifest2);
    assert(scene2 != nullptr);

    auto table2 = scene2->findByName("table");
    assert(table2 != nullptr);
    assert(table2->getType() == hse::PrimitiveType::Group);
    assert(table2->getChildren().size() == 5); // top + 4 legs

    // Check child transform propagation in reloaded scene
    auto leg = scene2->findByName("table_leg_1");
    assert(leg != nullptr);
    assert(leg->getParent() == table2);

    table2->setPosition({100, 0, 0});
    table2->computeWorldMatrix();
    // leg local was +/- 0.9. world should be 100 +/- 0.9
    assert(std::abs(leg->getWorldMatrix().data[12] - 99.1f) < 0.01f || std::abs(leg->getWorldMatrix().data[12] - 100.9f) < 0.01f);

    std::cout << "PASS: assembly persistence" << std::endl;
    return 0;
}
