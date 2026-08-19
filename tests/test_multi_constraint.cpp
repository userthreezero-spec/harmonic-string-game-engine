#include <cassert>
#include <iostream>
#include <memory>
#include <cmath>
#include <vector>
#include "scene/scene.h"
#include "scene/primitive.h"
#include "scene/scene_builder.h"
#include "math/bounding_box.h"

void test_final_conformance() {
    hse::ProjectManifest manifest;
    auto scene = hse::SceneBuilder::importState("C:/Users/Rico/HarmonicStringEngine/experiments/hse019/room.hsc", manifest);
    assert(scene != nullptr);

    auto chair1 = scene->findByName("chair_1");
    auto chair2 = scene->findByName("chair_2");
    auto table = scene->findByName("table_top");

    assert(chair1 && chair2 && table);

    // 1. Proximity Check
    std::cout << "Checking proximity..." << std::endl;
    float dist1 = (chair1->getPosition() - table->getPosition()).length();
    float dist2 = (chair2->getPosition() - table->getPosition()).length();
    std::cout << "  Dist 1: " << dist1 << " Dist 2: " << dist2 << std::endl;
    assert(dist1 < 1.0f); // Table center to chair center
    assert(dist2 < 1.0f);

    // 2. Collision Check (No object should hit wall right at x=3.0)
    std::cout << "Checking wall collisions..." << std::endl;
    for (auto& prim : scene->getPrimitives()) {
        hse::BoundingBox bb = prim->getBoundingBox();
        if (bb.max.x > 3.01f || bb.min.x < -3.01f || bb.max.z > 3.01f || bb.min.z < -3.01f) {
            std::cout << "  COLLISION: " << prim->getName() << " at " << bb.max.x << std::endl;
            assert(false);
        }
    }

    // 3. Doorway Access (Volume at z=3, x=[-1, 1] should be clear)
    std::cout << "Checking doorway access..." << std::endl;
    hse::BoundingBox doorVolume({-1.1f, -1.5f, 2.5f}, {1.1f, 1.0f, 3.5f});
    for (auto& prim : scene->getPrimitives()) {
        if (prim->getName().find("wall") != std::string::npos) continue;
        if (prim->getName() == "floor" || prim->getName() == "ceiling") continue;

        if (prim->getBoundingBox().intersects(doorVolume)) {
            std::cout << "  DOOR BLOCKED BY: " << prim->getName() << std::endl;
            assert(false);
        }
    }

    std::cout << "PASS: Multi-Constraint Conformance" << std::endl;
}

int main() {
    test_final_conformance();
    return 0;
}
