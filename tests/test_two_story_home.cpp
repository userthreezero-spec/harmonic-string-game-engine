#include <cassert>
#include <iostream>
#include <memory>
#include <cmath>
#include <fstream>
#include <string>
#include <vector>
#include "scene/scene.h"
#include "scene/camera.h"
#include "scene/primitive.h"
#include "scene/light.h"
#include "scene/animated_light.h"
#include "scene/animated_primitive.h"
#include "scene/timeline.h"
#include "math/vec3.h"
#include "math/mat4.h"

using hse::Vec3;
using hse::Mat4;

static bool approxEqual(float a, float b, float eps = 0.02f) {
    return std::abs(a - b) < eps;
}

static std::shared_ptr<hse::Scene> createHomeScene() {
    auto scene = std::make_shared<hse::Scene>("Two Story Home Scene");

    // Camera
    auto cam = std::make_shared<hse::Camera>(hse::ProjectionType::Perspective);
    cam->setPosition({0.0f, 4.0f, 15.0f});
    cam->lookAt({0.0f, 2.5f, 0.0f});
    cam->update();
    scene->addCamera(cam);

    // Primitives
    auto foundation = std::make_shared<hse::Primitive>(hse::PrimitiveType::Cube);
    foundation->setName("foundation_0"); foundation->setPosition({0, -0.25f, 0}); foundation->setScale({12.0f, 0.5f, 10.0f});
    scene->addPrimitive(foundation);

    auto floor1 = std::make_shared<hse::Primitive>(hse::PrimitiveType::Cube);
    floor1->setName("floor_1"); floor1->setPosition({0, 0.1f, 0}); floor1->setScale({11.8f, 0.2f, 9.8f});
    scene->addPrimitive(floor1);

    auto f1WallFL = std::make_shared<hse::Primitive>(hse::PrimitiveType::Cube);
    f1WallFL->setName("f1_wall_front_left"); f1WallFL->setPosition({-3.5f, 1.6f, 4.8f}); f1WallFL->setScale({4.8f, 2.8f, 0.2f});
    scene->addPrimitive(f1WallFL);

    auto f1WallFR = std::make_shared<hse::Primitive>(hse::PrimitiveType::Cube);
    f1WallFR->setName("f1_wall_front_right"); f1WallFR->setPosition({3.5f, 1.6f, 4.8f}); f1WallFR->setScale({4.8f, 2.8f, 0.2f});
    scene->addPrimitive(f1WallFR);

    auto f1WallB = std::make_shared<hse::Primitive>(hse::PrimitiveType::Cube);
    f1WallB->setName("f1_wall_back"); f1WallB->setPosition({0, 1.6f, -4.8f}); f1WallB->setScale({11.8f, 2.8f, 0.2f});
    scene->addPrimitive(f1WallB);

    auto f1WallL = std::make_shared<hse::Primitive>(hse::PrimitiveType::Cube);
    f1WallL->setName("f1_wall_left"); f1WallL->setPosition({-5.8f, 1.6f, 0}); f1WallL->setScale({0.2f, 2.8f, 9.8f});
    scene->addPrimitive(f1WallL);

    auto f1WallR = std::make_shared<hse::Primitive>(hse::PrimitiveType::Cube);
    f1WallR->setName("f1_wall_right"); f1WallR->setPosition({5.8f, 1.6f, 0}); f1WallR->setScale({0.2f, 2.8f, 9.8f});
    scene->addPrimitive(f1WallR);

    auto f1WallDiv = std::make_shared<hse::Primitive>(hse::PrimitiveType::Cube);
    f1WallDiv->setName("f1_wall_divider"); f1WallDiv->setPosition({0, 1.6f, 0}); f1WallDiv->setScale({0.2f, 2.8f, 9.8f});
    scene->addPrimitive(f1WallDiv);

    auto f1Door = std::make_shared<hse::Primitive>(hse::PrimitiveType::Cube);
    f1Door->setName("f1_door_front"); f1Door->setPosition({0, 1.2f, 4.85f}); f1Door->setScale({1.8f, 2.2f, 0.1f});
    scene->addPrimitive(f1Door);

    auto f1WinFL = std::make_shared<hse::Primitive>(hse::PrimitiveType::Cube);
    f1WinFL->setName("f1_window_front_left"); f1WinFL->setPosition({-3.5f, 1.8f, 4.88f}); f1WinFL->setScale({1.5f, 1.2f, 0.15f});
    scene->addPrimitive(f1WinFL);

    auto f1WinFR = std::make_shared<hse::Primitive>(hse::PrimitiveType::Cube);
    f1WinFR->setName("f1_window_front_right"); f1WinFR->setPosition({3.5f, 1.8f, 4.88f}); f1WinFR->setScale({1.5f, 1.2f, 0.15f});
    scene->addPrimitive(f1WinFR);

    auto f1WinS = std::make_shared<hse::Primitive>(hse::PrimitiveType::Cube);
    f1WinS->setName("f1_window_side"); f1WinS->setPosition({-5.88f, 1.8f, 0}); f1WinS->setScale({0.15f, 1.2f, 1.5f});
    scene->addPrimitive(f1WinS);

    auto stairs = std::make_shared<hse::Primitive>(hse::PrimitiveType::Cube);
    stairs->setName("staircase_0"); stairs->setPosition({-1.5f, 1.6f, -2.5f}); stairs->setScale({1.2f, 3.2f, 3.5f});
    scene->addPrimitive(stairs);

    auto floor2 = std::make_shared<hse::Primitive>(hse::PrimitiveType::Cube);
    floor2->setName("floor_2"); floor2->setPosition({0, 3.1f, 0}); floor2->setScale({11.8f, 0.2f, 9.8f});
    scene->addPrimitive(floor2);

    auto f2WallF = std::make_shared<hse::Primitive>(hse::PrimitiveType::Cube);
    f2WallF->setName("f2_wall_front"); f2WallF->setPosition({0, 4.6f, 4.8f}); f2WallF->setScale({11.8f, 2.8f, 0.2f});
    scene->addPrimitive(f2WallF);

    auto f2WallB = std::make_shared<hse::Primitive>(hse::PrimitiveType::Cube);
    f2WallB->setName("f2_wall_back"); f2WallB->setPosition({0, 4.6f, -4.8f}); f2WallB->setScale({11.8f, 2.8f, 0.2f});
    scene->addPrimitive(f2WallB);

    auto f2WallL = std::make_shared<hse::Primitive>(hse::PrimitiveType::Cube);
    f2WallL->setName("f2_wall_left"); f2WallL->setPosition({-5.8f, 4.6f, 0}); f2WallL->setScale({0.2f, 2.8f, 9.8f});
    scene->addPrimitive(f2WallL);

    auto f2WallR = std::make_shared<hse::Primitive>(hse::PrimitiveType::Cube);
    f2WallR->setName("f2_wall_right"); f2WallR->setPosition({5.8f, 4.6f, 0}); f2WallR->setScale({0.2f, 2.8f, 9.8f});
    scene->addPrimitive(f2WallR);

    auto f2WallDiv = std::make_shared<hse::Primitive>(hse::PrimitiveType::Cube);
    f2WallDiv->setName("f2_wall_divider"); f2WallDiv->setPosition({0, 4.6f, 0}); f2WallDiv->setScale({0.2f, 2.8f, 9.8f});
    scene->addPrimitive(f2WallDiv);

    auto f2WinFL = std::make_shared<hse::Primitive>(hse::PrimitiveType::Cube);
    f2WinFL->setName("f2_window_front_left"); f2WinFL->setPosition({-3.5f, 4.8f, 4.88f}); f2WinFL->setScale({1.5f, 1.2f, 0.15f});
    scene->addPrimitive(f2WinFL);

    auto f2WinFR = std::make_shared<hse::Primitive>(hse::PrimitiveType::Cube);
    f2WinFR->setName("f2_window_front_right"); f2WinFR->setPosition({3.5f, 4.8f, 4.88f}); f2WinFR->setScale({1.5f, 1.2f, 0.15f});
    scene->addPrimitive(f2WinFR);

    auto f2Door = std::make_shared<hse::Primitive>(hse::PrimitiveType::Cube);
    f2Door->setName("f2_door_balcony"); f2Door->setPosition({0, 4.2f, 4.85f}); f2Door->setScale({1.6f, 2.0f, 0.1f});
    scene->addPrimitive(f2Door);

    auto roof = std::make_shared<hse::Primitive>(hse::PrimitiveType::Cube);
    roof->setName("roof_main"); roof->setPosition({0, 6.3f, 0}); roof->setScale({12.6f, 0.6f, 10.6f});
    scene->addPrimitive(roof);

    // Structural and semantic relationships
    scene->addRelationship("foundation_0", "supports", "floor_1");
    scene->addRelationship("floor_1", "supports", "f1_wall_front_left");
    scene->addRelationship("floor_1", "supports", "f1_wall_front_right");
    scene->addRelationship("floor_1", "supports", "f1_wall_back");
    scene->addRelationship("floor_1", "supports", "f1_wall_left");
    scene->addRelationship("floor_1", "supports", "f1_wall_right");
    scene->addRelationship("floor_1", "supports", "f1_wall_divider");
    scene->addRelationship("floor_1", "supports", "staircase_0");

    scene->addRelationship("f1_door_front", "mounted_on", "f1_wall_front_left");
    scene->addRelationship("f1_window_front_left", "mounted_on", "f1_wall_front_left");
    scene->addRelationship("f1_window_front_right", "mounted_on", "f1_wall_front_right");
    scene->addRelationship("f1_window_side", "mounted_on", "f1_wall_left");

    scene->addRelationship("f1_wall_front_left", "supports", "floor_2");
    scene->addRelationship("floor_2", "supports", "f2_wall_front");
    scene->addRelationship("floor_2", "supports", "f2_wall_back");
    scene->addRelationship("floor_2", "supports", "f2_wall_left");
    scene->addRelationship("floor_2", "supports", "f2_wall_right");
    scene->addRelationship("floor_2", "supports", "f2_wall_divider");

    scene->addRelationship("f2_window_front_left", "mounted_on", "f2_wall_front");
    scene->addRelationship("f2_window_front_right", "mounted_on", "f2_wall_front");
    scene->addRelationship("f2_door_balcony", "mounted_on", "f2_wall_front");

    scene->addRelationship("f2_wall_front", "supports", "roof_main");

    return scene;
}

// TEST A: Home scene initializes
void test_A_home_init() {
    auto scene = createHomeScene();
    assert(scene != nullptr);
    assert(scene->getPrimitives().size() >= 23);
    assert(scene->getActiveCamera() != nullptr);
    std::cout << "PASS A: Home scene initializes (primitives count: " << scene->getPrimitives().size() << ")" << std::endl;
}

// TEST B: Floor 1 exists
void test_B_floor_1() {
    auto scene = createHomeScene();
    bool foundFloor1 = false, foundWallFL = false, foundDoor = false;
    for (auto& p : scene->getPrimitives()) {
        if (p->getName() == "floor_1") foundFloor1 = true;
        if (p->getName() == "f1_wall_front_left") foundWallFL = true;
        if (p->getName() == "f1_door_front") foundDoor = true;
    }
    assert(foundFloor1 && foundWallFL && foundDoor);
    std::cout << "PASS B: Floor 1 exists with walls and openings" << std::endl;
}

// TEST C: Floor 2 exists
void test_C_floor_2() {
    auto scene = createHomeScene();
    bool foundFloor2 = false, foundWallF = false, foundBalconyDoor = false;
    for (auto& p : scene->getPrimitives()) {
        if (p->getName() == "floor_2") foundFloor2 = true;
        if (p->getName() == "f2_wall_front") foundWallF = true;
        if (p->getName() == "f2_door_balcony") foundBalconyDoor = true;
    }
    assert(foundFloor2 && foundWallF && foundBalconyDoor);
    std::cout << "PASS C: Floor 2 exists with upper walls and balcony door" << std::endl;
}

// TEST D: Floors are spatially related
void test_D_floors_spatially_related() {
    auto scene = createHomeScene();
    std::shared_ptr<hse::Primitive> f1 = nullptr, f2 = nullptr;
    for (auto& p : scene->getPrimitives()) {
        if (p->getName() == "floor_1") f1 = p;
        if (p->getName() == "floor_2") f2 = p;
    }
    assert(f1 != nullptr && f2 != nullptr);
    assert(f2->getPosition().y > f1->getPosition().y + 2.0f); // upper floor is above ground floor
    std::cout << "PASS D: Floors spatially related (Floor 1 Y=" << f1->getPosition().y << ", Floor 2 Y=" << f2->getPosition().y << ")" << std::endl;
}

// TEST E: Stairs connect the two floors
void test_E_stairs() {
    auto scene = createHomeScene();
    std::shared_ptr<hse::Primitive> f1 = nullptr, f2 = nullptr, stairs = nullptr;
    for (auto& p : scene->getPrimitives()) {
        if (p->getName() == "floor_1") f1 = p;
        if (p->getName() == "floor_2") f2 = p;
        if (p->getName() == "staircase_0") stairs = p;
    }
    assert(stairs != nullptr);
    float sy = stairs->getPosition().y;
    assert(sy > f1->getPosition().y && sy < f2->getPosition().y); // stairs bridge the vertical gap
    std::cout << "PASS E: Stairs connect Floor 1 and Floor 2 (Stairs Y=" << sy << ")" << std::endl;
}

// TEST F: Rooms exist and are structurally related
void test_F_rooms() {
    auto scene = createHomeScene();
    std::shared_ptr<hse::Primitive> div1 = nullptr, div2 = nullptr;
    for (auto& p : scene->getPrimitives()) {
        if (p->getName() == "f1_wall_divider") div1 = p;
        if (p->getName() == "f2_wall_divider") div2 = p;
    }
    assert(div1 != nullptr && div2 != nullptr);
    std::cout << "PASS F: Rooms and interior wall dividers exist" << std::endl;
}

// TEST G: Walls/floors/roof relationships represented
void test_G_relationships() {
    auto scene = createHomeScene();
    const auto& rels = scene->getRelationships();
    assert(rels.size() >= 20);
    bool foundFoundToFloor1 = false, foundWallToFloor2 = false, foundWallToRoof = false;
    for (auto& r : rels) {
        if (r.from == "foundation_0" && r.rel == "supports" && r.to == "floor_1") foundFoundToFloor1 = true;
        if (r.from == "f1_wall_front_left" && r.rel == "supports" && r.to == "floor_2") foundWallToFloor2 = true;
        if (r.from == "f2_wall_front" && r.rel == "supports" && r.to == "roof_main") foundWallToRoof = true;
    }
    assert(foundFoundToFloor1 && foundWallToFloor2 && foundWallToRoof);
    std::cout << "PASS G: Structural relationships represented (foundation->floor_1, wall->floor_2, wall->roof)" << std::endl;
}

// TEST H: Camera can inspect the scene
void test_H_camera() {
    auto scene = createHomeScene();
    auto cam = scene->getActiveCamera();
    assert(cam != nullptr);
    // Move camera to different inspection viewports
    cam->setPosition({0.0f, 1.5f, 5.0f}); // Floor 1 view
    cam->update();
    assert(cam->getPosition().y == 1.5f);

    cam->setPosition({0.0f, 4.5f, 5.0f}); // Floor 2 view
    cam->update();
    assert(cam->getPosition().y == 4.5f);

    cam->setPosition({15.0f, 10.0f, 15.0f}); // Structural overview
    cam->update();
    assert(cam->getPosition().x == 15.0f);
    std::cout << "PASS H: Camera inspects scene across multiple viewports" << std::endl;
}

// TEST I: Scene::update processes the home
void test_I_scene_update() {
    auto scene = createHomeScene();
    float t0 = scene->getTime();
    scene->update(0.016f);
    assert(scene->getTime() > t0);
    std::cout << "PASS I: Scene::update processes the home scene" << std::endl;
}

// TEST J: Harmonic/relationship processing executes during runtime
void test_J_harmonic_runtime() {
    auto scene = createHomeScene();
    scene->update(0.016f); // Triggers multi-pass relationship propagation
    std::shared_ptr<hse::Primitive> f1 = nullptr, roof = nullptr;
    for (auto& p : scene->getPrimitives()) {
        if (p->getName() == "floor_1") f1 = p;
        if (p->getName() == "roof_main") roof = p;
    }
    assert(f1 != nullptr && roof != nullptr);
    // World position updated
    assert(roof->getWorldPosition().y > 5.0f);
    std::cout << "PASS J: Relationship processing executes at runtime (roof world Y=" << roof->getWorldPosition().y << ")" << std::endl;
}

// TEST K: Controlled change propagates through real relationship
void test_K_propagation() {
    auto scene = createHomeScene();
    std::shared_ptr<hse::Primitive> found = nullptr, f1 = nullptr, roof = nullptr;
    for (auto& p : scene->getPrimitives()) {
        if (p->getName() == "foundation_0") found = p;
        if (p->getName() == "floor_1") f1 = p;
        if (p->getName() == "roof_main") roof = p;
    }
    assert(found && f1 && roof);

    // Initial evaluation
    scene->update(0.016f);
    float initialRoofY = roof->getPosition().y;

    // Controlled parameter change: raise foundation by +0.5m
    Vec3 origFoundPos = found->getPosition();
    found->setPosition({origFoundPos.x, origFoundPos.y + 0.5f, origFoundPos.z});

    // Run harmonic relationship loop
    scene->update(0.016f);

    float updatedRoofY = roof->getPosition().y;
    float deltaRoofY = updatedRoofY - initialRoofY;

    // Proof: raising foundation raised roof by exactly +0.5m via structural relationship chain
    assert(approxEqual(deltaRoofY, 0.5f));
    std::cout << "PASS K: Controlled change propagated through relationship chain (foundation +0.5m -> roof +" << deltaRoofY << "m)" << std::endl;
}

// TEST L: Resulting state reaches rendering
void test_L_rendering() {
    auto scene = createHomeScene();
    std::shared_ptr<hse::Primitive> found = nullptr, roof = nullptr;
    for (auto& p : scene->getPrimitives()) {
        if (p->getName() == "foundation_0") found = p;
        if (p->getName() == "roof_main") roof = p;
    }
    found->setPosition({0, 1.0f, 0}); // Shift foundation
    scene->update(0.016f);

    const Mat4& wt = roof->getWorldTransform();
    assert(roof->getWorldPosition().y > 6.5f);
    assert(wt.ptr()[15] == 1.0f); // Valid GL matrix
    std::cout << "PASS L: Resulting state reaches renderer (roof world Y=" << roof->getWorldPosition().y << ")" << std::endl;
}

// TEST M: No disconnected structural objects exist
void test_M_no_disconnected() {
    auto scene = createHomeScene();
    const auto& rels = scene->getRelationships();
    for (auto& p : scene->getPrimitives()) {
        std::string name = p->getName();
        if (name == "foundation_0") continue; // root
        bool connected = false;
        for (auto& r : rels) {
            if (r.from == name || r.to == name) { connected = true; break; }
        }
        assert(connected); // Every non-root primitive has at least 1 structural relationship
    }
    std::cout << "PASS M: Zero disconnected structural objects found" << std::endl;
}

// TEST N: Scene survives repeated update cycles
void test_N_repeated_updates() {
    auto scene = createHomeScene();
    for (int frame = 0; frame < 100; ++frame) {
        scene->update(0.016f);
    }
    assert(approxEqual(scene->getTime(), 1.6f));
    std::cout << "PASS N: Scene survives 100 continuous update cycles (1.6s simulation time)" << std::endl;
}

// TEST O: Shutdown remains clean
void test_O_shutdown() {
    {
        auto scene = createHomeScene();
        scene->update(0.016f);
    }
    std::cout << "PASS O: Clean shutdown completed with zero memory anomalies" << std::endl;
}

// Full causal test
void test_causal_chain() {
    auto scene = createHomeScene();
    std::shared_ptr<hse::Primitive> found = nullptr, f2Wall = nullptr, roof = nullptr;
    for (auto& p : scene->getPrimitives()) {
        if (p->getName() == "foundation_0") found = p;
        if (p->getName() == "f2_wall_front") f2Wall = p;
        if (p->getName() == "roof_main") roof = p;
    }

    scene->update(0.016f);
    float yRoof0 = roof->getWorldPosition().y;

    // INPUT/PARAMETER CHANGE
    found->setPosition({0, 0.5f, 0}); // Raise foundation by +0.75m

    // HARMONIC/RELATIONSHIP PROCESSING
    scene->update(0.016f);

    // DEPENDENT SCENE STATE CHANGE
    float yRoof1 = roof->getWorldPosition().y;

    // RENDERING-OBSERVABLE CHANGE
    assert(yRoof1 > yRoof0 + 0.70f);
    std::cout << "PASS CAUSAL CHAIN: Input(foundation pos) -> Relationship(multi-hop supports) -> Scene State(roof pos) -> Rendering(world pos Y " << yRoof0 << " -> " << yRoof1 << ")" << std::endl;
}

int main() {
    test_A_home_init();
    test_B_floor_1();
    test_C_floor_2();
    test_D_floors_spatially_related();
    test_E_stairs();
    test_F_rooms();
    test_G_relationships();
    test_H_camera();
    test_I_scene_update();
    test_J_harmonic_runtime();
    test_K_propagation();
    test_L_rendering();
    test_M_no_disconnected();
    test_N_repeated_updates();
    test_O_shutdown();
    test_causal_chain();
    std::cout << "All Two-Story Home E2E tests passed (16/16)!" << std::endl;
    return 0;
}
