#include <cassert>
#include <iostream>
#include <memory>
#include <cmath>
#include "scene/scene.h"
#include "scene/primitive.h"
#include "scene/scene_builder.h"

void test_conversation_setup() {
    hse::ProjectManifest manifest;
    auto scene = hse::SceneBuilder::importState("C:/Users/Rico/HarmonicStringEngine/experiments/hse019/room.hsc", manifest);
    assert(scene != nullptr);

    auto chair1 = scene->findByName("chair_1");
    auto chair2 = scene->findByName("chair_2");
    auto table = scene->findByName("table_top");

    assert(chair1 && chair2 && table);

    // 1. Stability Check
    std::cout << "Checking stability..." << std::endl;
    assert(chair1->getRotationSpeed().length() == 0.0f);
    assert(chair2->getRotationSpeed().length() == 0.0f);

    // 2. Proximity Check
    // Table is 2.0m wide (x-axis), center at 0. Edges at +/- 1.0.
    // Chairs should be close to +/- 1.0.
    std::cout << "Checking proximity..." << std::endl;
    float dist1 = std::abs(chair1->getPosition().x - (-1.0f));
    float dist2 = std::abs(chair2->getPosition().x - 1.0f);

    std::cout << "  Chair 1 dist to edge: " << dist1 << std::endl;
    std::cout << "  Chair 2 dist to edge: " << dist2 << std::endl;

    assert(dist1 < 0.3f);
    assert(dist2 < 0.3f);

    // 3. Facing Check
    // Chair 1 should face +X (approx 90 deg)
    // Chair 2 should face -X (approx -90 deg)
    std::cout << "Checking facing..." << std::endl;
    assert(std::abs(chair1->getRotation().y - 90.0f) < 1.0f);
    assert(std::abs(chair2->getRotation().y - (-90.0f)) < 1.0f);

    std::cout << "PASS: Goal Conformance (Conversation Setup)" << std::endl;
}

int main() {
    test_conversation_setup();
    return 0;
}
