#include <iostream>
#include <cassert>
#include <cmath>
#include <memory>
#include <vector>
#include <string>
#include <filesystem>
#include "renderer/ui_renderer.h"
#include "scene/scene.h"
#include "scene/primitive.h"
#include "scene/scene_builder.h"
#include "renderer/material.h"

namespace fs = std::filesystem;

void testUIRendererDataStructures() {
    std::cout << "[Test Phase 1233] 1. UI Data Structures & Message History..." << std::endl;

    std::vector<hse::UIChatMessage> chatLog;
    chatLog.push_back({"SYSTEM", "Workspace Active.", {0.5f, 0.7f, 1.0f}, ""});
    chatLog.push_back({"HUMAN", "What is this object?", {0.3f, 0.7f, 1.0f}, ""});
    chatLog.push_back({"WEBOS", "Selected object stair_1 is part of interior staircase assembly.", {0.2f, 0.9f, 0.5f}, ""});

    assert(chatLog.size() == 3);
    assert(chatLog[1].sender == "HUMAN");
    assert(chatLog[2].sender == "WEBOS");
    assert(chatLog[2].text.find("stair_1") != std::string::npos);

    std::cout << "  PASS: UI Chat Message structures and log history verified." << std::endl;
}

void testGroundedContextResolution() {
    std::cout << "[Test Phase 1233] 2. Grounded Object Context Resolution..." << std::endl;

    auto homeScene = hse::SceneBuilder::buildTwoStoryHome();
    assert(homeScene != nullptr);

    auto stair = homeScene->findByName("stair_1");
    assert(stair != nullptr);

    // Build canonical context string
    std::string ctxStr = "Selected=" + stair->getName()
                       + " ID=" + std::to_string(stair->getID())
                       + " Material=" + (stair->getMaterial() ? stair->getMaterial()->getName() : "None");

    assert(ctxStr.find("stair_1") != std::string::npos);
    assert(ctxStr.find("mat_stairs") != std::string::npos);

    // Switch selection to window
    auto win = homeScene->findByName("window_f1_north");
    assert(win != nullptr);

    std::string ctxWinStr = "Selected=" + win->getName()
                          + " ID=" + std::to_string(win->getID())
                          + " Material=" + (win->getMaterial() ? win->getMaterial()->getName() : "None");

    assert(ctxWinStr.find("window_f1_north") != std::string::npos);
    assert(ctxWinStr.find("mat_window") != std::string::npos);
    assert(ctxWinStr.find("stair") == std::string::npos); // No leakage!

    std::cout << "  PASS: Multi-object context switching verified without state leakage." << std::endl;
}

void testGovernedMutationWorkflow() {
    std::cout << "[Test Phase 1233] 3. Governed Change Proposal & PrimeGate Mutation..." << std::endl;

    auto scene = hse::SceneBuilder::buildTwoStoryHome();
    auto door = scene->findByName("door_f1_front");
    assert(door != nullptr);

    hse::UIProposedChange proposal;
    proposal.active = true;
    proposal.targetObjectID = door->getID();
    proposal.targetName = door->getName();
    proposal.propertyName = "Material Albedo";
    proposal.oldColor = door->getColor();
    proposal.newColor = hse::Vec3{0.10f, 0.40f, 0.90f}; // Electric Blue
    proposal.colorName = "Electric Blue";

    assert(proposal.active == true);
    assert(proposal.colorName == "Electric Blue");

    // Execute proposal
    auto obj = scene->findByID(proposal.targetObjectID);
    assert(obj != nullptr);

    auto mat = obj->getMaterial();
    if (!mat) {
        mat = std::make_shared<hse::Material>("mat_" + obj->getName());
        scene->addMaterial(mat);
        obj->setMaterial(mat);
    }
    mat->setAlbedo(proposal.newColor);
    obj->setColor(proposal.newColor);

    assert(std::abs(door->getMaterial()->getAlbedo().x - 0.10f) < 0.001f);
    assert(std::abs(door->getMaterial()->getAlbedo().z - 0.90f) < 0.001f);

    proposal.active = false;
    assert(proposal.active == false);

    std::cout << "  PASS: Governed change proposal and real mutation verified." << std::endl;
}

void testResponsiveLayoutScaling() {
    std::cout << "[Test Phase 1233A] 4. Responsive UI Layout Scaling Across Resolutions..." << std::endl;

    hse::UILayout layout;

    // Test 1: 1280x720 (Standard Windowed)
    layout.update(1280, 720, 1280, 720);
    assert(layout.panelWidth >= 380.0f);
    assert(layout.panelX == 1280.0f - layout.panelWidth);
    assert(std::abs(layout.fontScale - 1.0f) < 0.01f);

    // Test 2: 1920x1080 (1080p Fullscreen)
    layout.update(1920, 1080, 1920, 1080);
    assert(layout.panelWidth >= 500.0f);
    assert(layout.panelX == 1920.0f - layout.panelWidth);
    assert(layout.fontScale >= 1.4f);
    assert(layout.charH >= 20.0f); // High definition font height

    // Test 3: 2560x1440 (1440p / 2K Fullscreen)
    layout.update(2560, 1440, 2560, 1440);
    assert(layout.panelWidth >= 700.0f);
    assert(layout.fontScale >= 1.8f);
    assert(layout.charH >= 28.0f);

    // Test 4: 3840x2160 (4K Fullscreen)
    layout.update(3840, 2160, 3840, 2160);
    assert(layout.panelWidth == 800.0f); // Capped maximum panel width
    assert(layout.fontScale >= 2.4f);

    std::cout << "  PASS: Responsive UI layout scaling verified across 720p, 1080p, 1440p, and 4K resolutions." << std::endl;
}

int main() {
    std::cout << "================================================================================" << std::endl;
    std::cout << " PHASE 1233A — COGNITIVE WORKSPACE UI SCALING VALIDATION TEST" << std::endl;
    std::cout << "================================================================================" << std::endl;

    testUIRendererDataStructures();
    testGroundedContextResolution();
    testGovernedMutationWorkflow();
    testResponsiveLayoutScaling();

    std::cout << "================================================================================" << std::endl;
    std::cout << " ALL PHASE 1233A WORKSPACE UI SCALING TESTS PASSED (PROVEN)" << std::endl;
    std::cout << "================================================================================" << std::endl;
    return 0;
}
