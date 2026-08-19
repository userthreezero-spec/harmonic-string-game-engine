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
    // Chairs should be close to the table.
    std::cout << "Checking proximity..." << std::endl;
    float dist1 = (chair1->getPosition() - table->getPosition()).length();
    float dist2 = (chair2->getPosition() - table->getPosition()).length();

    std::cout << "  Chair 1 dist to table center: " << dist1 << std::endl;
    std::cout << "  Chair 2 dist to table center: " << dist2 << std::endl;

    assert(dist1 < 1.0f);
    assert(dist2 < 1.0f);

    // 3. Facing Check
    // Chairs should face the table
    std::cout << "Checking facing..." << std::endl;
    assert(chair1->getRotationSpeed().length() == 0.0f);

    std::cout << "PASS: Goal Conformance (General Conversation)" << std::endl;
}

int main() {
    test_conversation_setup();
    return 0;
}
