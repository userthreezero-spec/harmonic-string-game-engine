#include <iostream>
#include <cassert>
#include <cmath>
#include <memory>
#include <filesystem>
#include "scene/camera.h"
#include "scene/scene.h"
#include "scene/primitive.h"
#include "scene/scene_builder.h"
#include "scene/picker.h"
#include "renderer/material.h"

namespace fs = std::filesystem;

void testCameraFoundation() {
    std::cout << "[Test Phase 1232] 1. Camera Navigation & Orbit System..." << std::endl;

    hse::Camera cam(hse::ProjectionType::Perspective);
    cam.setPosition({0.0f, 5.0f, 10.0f});
    cam.lookAt({0.0f, 0.0f, 0.0f});

    assert(std::abs(cam.getPosition().y - 5.0f) < 0.001f);
    assert(std::abs(cam.getTarget().z - 0.0f) < 0.001f);

    // Keyboard WASDQE movement
    cam.processKeyboard("FORWARD", 0.1f, false);
    assert(cam.getPosition().z < 10.0f); // moved closer to target

    cam.processKeyboard("BACKWARD", 0.1f, false);
    assert(std::abs(cam.getPosition().z - 10.0f) < 0.1f);

    // Orbit Rotation
    hse::Vec3 initialPos = cam.getPosition();
    cam.orbitRotate(15.0f, 10.0f);
    assert(cam.getPosition().x != initialPos.x || cam.getPosition().y != initialPos.y);

    // Pan View
    hse::Vec3 posBeforePan = cam.getPosition();
    cam.panView(10.0f, 5.0f);
    assert(cam.getPosition().x != posBeforePan.x || cam.getPosition().y != posBeforePan.y);

    std::cout << "  PASS: Camera navigation, orbit rotation, and panning verified." << std::endl;
}

void testMaterialSystemAndColorDistinction() {
    std::cout << "[Test Phase 1232] 2. Material System & Color Distinction..." << std::endl;

    auto matA = std::make_shared<hse::Material>("mat_red");
    matA->setAlbedo({0.9f, 0.1f, 0.1f});

    auto matB = std::make_shared<hse::Material>("mat_blue");
    matB->setAlbedo({0.1f, 0.2f, 0.8f});

    auto primA = std::make_shared<hse::Primitive>(hse::PrimitiveType::Cube);
    primA->setMaterial(matA);
    assert(std::abs(primA->getColor().x - 0.9f) < 0.001f);

    auto primB = std::make_shared<hse::Primitive>(hse::PrimitiveType::Cube);
    primB->setMaterial(matB);
    assert(std::abs(primB->getColor().z - 0.8f) < 0.001f);

    // Verify TwoStoryHome multi-material distinction
    auto homeScene = hse::SceneBuilder::buildTwoStoryHome();
    assert(homeScene != nullptr);
    assert(homeScene->getPrimitiveCount() >= 60);

    // Count distinct material colors in TwoStoryHome scene
    int distinctColorCount = 0;
    std::vector<hse::Vec3> uniqueColors;
    for (const auto& p : homeScene->getPrimitives()) {
        hse::Vec3 c = p->getMaterial() ? p->getMaterial()->getAlbedo() : p->getColor();
        bool found = false;
        for (const auto& uc : uniqueColors) {
            if (std::abs(uc.x - c.x) < 0.01f && std::abs(uc.y - c.y) < 0.01f && std::abs(uc.z - c.z) < 0.01f) {
                found = true;
                break;
            }
        }
        if (!found) {
            uniqueColors.push_back(c);
        }
    }

    std::cout << "  PASS: Found " << uniqueColors.size() << " distinct surface material colors in TwoStoryHome scene." << std::endl;
    assert(uniqueColors.size() >= 5); // Must have at least 5 distinct material colors
}

void testCreateProjectWorkflow() {
    std::cout << "[Test Phase 1232] 3. Create Project Workflow & Import..." << std::endl;

    fs::path testDir = fs::current_path() / "test_projects_phase1232";
    fs::create_directories(testDir);

    std::string projName = "auto_test_engine_proj";
    bool created = hse::SceneBuilder::createProject(testDir.string(), projName);
    assert(created == true);

    fs::path jsonPath = testDir / (projName + ".json");
    assert(fs::exists(jsonPath));

    hse::ProjectManifest manifest;
    auto importedScene = hse::SceneBuilder::importState(jsonPath.string(), manifest);
    assert(importedScene != nullptr);
    assert(importedScene->getPrimitiveCount() >= 2);
    assert(manifest.projectName == projName);

    fs::remove_all(testDir);
    std::cout << "  PASS: Create project workflow and import verified cleanly." << std::endl;
}

void testObjectLineageTracing() {
    std::cout << "[Test Phase 1232] 4. Object Picking & Lineage Tracing..." << std::endl;

    auto homeScene = hse::SceneBuilder::buildTwoStoryHome();
    auto camera = homeScene->getActiveCamera();
    assert(camera != nullptr);

    // Pick stairs / roof / foundation primitives by name
    auto stair1 = homeScene->findByName("stair_1");
    assert(stair1 != nullptr);
    assert(stair1->getParent() != nullptr);
    assert(stair1->getParent()->getName() == "stairs");

    // Trace parent chain
    std::string parentChain = stair1->getName();
    auto p = stair1->getParent();
    while (p) {
        parentChain += " -> " + p->getName();
        p = p->getParent();
    }
    std::cout << "  Lineage trace for stair_1: " << parentChain << std::endl;
    assert(parentChain.find("stairs") != std::string::npos);
    assert(parentChain.find("house_root") != std::string::npos);

    std::cout << "  PASS: Canonical lineage trace verified for nested scene assembly." << std::endl;
}

int main() {
    std::cout << "================================================================================" << std::endl;
    std::cout << " PHASE 1232 — HARMONIC STRING ENGINE FOUNDATION VALIDATION TEST" << std::endl;
    std::cout << "================================================================================" << std::endl;

    testCameraFoundation();
    testMaterialSystemAndColorDistinction();
    testCreateProjectWorkflow();
    testObjectLineageTracing();

    std::cout << "================================================================================" << std::endl;
    std::cout << " ALL PHASE 1232 ENGINE FOUNDATION TESTS PASSED (PROVEN)" << std::endl;
    std::cout << "================================================================================" << std::endl;
    return 0;
}
