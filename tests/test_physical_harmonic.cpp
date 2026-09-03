#include <cassert>
#include <iostream>
#include <memory>
#include <cmath>
#include <vector>
#include <string>
#include "scene/scene.h"
#include "scene/camera.h"
#include "scene/primitive.h"
#include "math/vec3.h"
#include "math/mat4.h"

using hse::Vec3;
using hse::Mat4;

static bool approxEqual(float a, float b, float eps = 0.05f) {
    return std::abs(a - b) < eps;
}

// TEST 1: Physical State Initialization
void test_1_physical_state_init() {
    hse::Primitive p(hse::PrimitiveType::Cube);
    p.setPosition({0.0f, 2.0f, 0.0f});
    p.setRestPosition({0.0f, 2.0f, 0.0f});
    p.setMass(2.5f);
    p.setSelfStiffness(200.0f);
    p.setSelfDamping(5.0f);

    assert(p.getMass() == 2.5f);
    assert(p.getRestPosition().y == 2.0f);
    assert(p.getSelfStiffness() == 200.0f);
    assert(p.getSelfDamping() == 5.0f);
    assert(p.getVelocity().y == 0.0f);
    assert(p.getKineticEnergy() == 0.0f);
    assert(p.getPotentialEnergy() == 0.0f);
    std::cout << "PASS 1: Physical state initialization verified" << std::endl;
}

// TEST 2: Deterministic Semi-Implicit Euler Integration
void test_2_integration() {
    auto scene = std::make_shared<hse::Scene>("Integration Test");
    auto p = std::make_shared<hse::Primitive>(hse::PrimitiveType::Cube);
    p->setPosition({0.0f, 0.0f, 0.0f});
    p->setMass(2.0f);
    p->setSelfStiffness(0.0f);
    p->setSelfDamping(0.0f);
    scene->addPrimitive(p);

    // Apply constant upward force 10 N
    p->addForce({0.0f, 10.0f, 0.0f});
    scene->update(0.1f); // acc = 10/2 = 5 m/s^2, vel = 0.5 m/s, pos = 0.05m

    assert(approxEqual(p->getAcceleration().y, 5.0f));
    assert(approxEqual(p->getVelocity().y, 0.5f));
    assert(approxEqual(p->getPosition().y, 0.05f));
    std::cout << "PASS 2: Deterministic semi-implicit Euler integration verified" << std::endl;
}

// TEST 3: Stiffness Sensitivity Experiment (k)
void test_3_stiffness_experiment() {
    // High stiffness vs Low stiffness
    auto scene1 = std::make_shared<hse::Scene>("Low K");
    auto p1 = std::make_shared<hse::Primitive>(hse::PrimitiveType::Cube);
    p1->setPosition({0.0f, 1.0f, 0.0f});
    p1->setRestPosition({0.0f, 0.0f, 0.0f}); // Displacement d = 1.0m
    p1->setSelfStiffness(50.0f); p1->setSelfDamping(0.0f);
    scene1->addPrimitive(p1);

    auto scene2 = std::make_shared<hse::Scene>("High K");
    auto p2 = std::make_shared<hse::Primitive>(hse::PrimitiveType::Cube);
    p2->setPosition({0.0f, 1.0f, 0.0f});
    p2->setRestPosition({0.0f, 0.0f, 0.0f}); // Displacement d = 1.0m
    p2->setSelfStiffness(500.0f); p2->setSelfDamping(0.0f);
    scene2->addPrimitive(p2);

    scene1->update(0.016f);
    scene2->update(0.016f);

    // High K produces 10x greater restoring acceleration
    assert(std::abs(p2->getAcceleration().y) > std::abs(p1->getAcceleration().y) * 5.0f);
    std::cout << "PASS 3: Stiffness sensitivity (k) verified (acc low=" << p1->getAcceleration().y << ", high=" << p2->getAcceleration().y << ")" << std::endl;
}

// TEST 4: Damping Sensitivity Experiment (c)
void test_4_damping_experiment() {
    auto sceneUndamped = std::make_shared<hse::Scene>("Undamped");
    auto p1 = std::make_shared<hse::Primitive>(hse::PrimitiveType::Cube);
    p1->setPosition({0.0f, 1.0f, 0.0f}); p1->setRestPosition({0.0f, 0.0f, 0.0f});
    p1->setSelfStiffness(100.0f); p1->setSelfDamping(0.0f);
    sceneUndamped->addPrimitive(p1);

    auto sceneDamped = std::make_shared<hse::Scene>("Damped");
    auto p2 = std::make_shared<hse::Primitive>(hse::PrimitiveType::Cube);
    p2->setPosition({0.0f, 1.0f, 0.0f}); p2->setRestPosition({0.0f, 0.0f, 0.0f});
    p2->setSelfStiffness(100.0f); p2->setSelfDamping(15.0f);
    sceneDamped->addPrimitive(p2);

    for (int i = 0; i < 50; ++i) {
        sceneUndamped->update(0.016f);
        sceneDamped->update(0.016f);
    }

    // Damped system energy decays significantly compared to undamped
    float e1 = sceneUndamped->getTotalSystemEnergy();
    float e2 = sceneDamped->getTotalSystemEnergy();
    assert(e2 < e1 * 0.2f);
    std::cout << "PASS 4: Damping sensitivity (c) verified (Energy undamped=" << e1 << "J, damped=" << e2 << "J)" << std::endl;
}

// TEST 5: Mass Sensitivity Experiment (m)
void test_5_mass_experiment() {
    auto sceneLight = std::make_shared<hse::Scene>("Light Mass");
    auto pLight = std::make_shared<hse::Primitive>(hse::PrimitiveType::Cube);
    pLight->setMass(1.0f); pLight->setPosition({0, 1.0f, 0}); pLight->setRestPosition({0, 0, 0});
    pLight->setSelfStiffness(100.0f);
    sceneLight->addPrimitive(pLight);

    auto sceneHeavy = std::make_shared<hse::Scene>("Heavy Mass");
    auto pHeavy = std::make_shared<hse::Primitive>(hse::PrimitiveType::Cube);
    pHeavy->setMass(10.0f); pHeavy->setPosition({0, 1.0f, 0}); pHeavy->setRestPosition({0, 0, 0});
    pHeavy->setSelfStiffness(100.0f);
    sceneHeavy->addPrimitive(pHeavy);

    sceneLight->update(0.016f);
    sceneHeavy->update(0.016f);

    // Light mass accelerates 10x faster for same spring force
    assert(std::abs(pLight->getAcceleration().y) > std::abs(pHeavy->getAcceleration().y) * 8.0f);
    std::cout << "PASS 5: Mass sensitivity (m) verified (acc light=" << pLight->getAcceleration().y << ", heavy=" << pHeavy->getAcceleration().y << ")" << std::endl;
}

// TEST 6: Multi-Entity Coupled Physical Propagation
void test_6_coupled_propagation() {
    auto scene = std::make_shared<hse::Scene>("Coupled Home");
    auto found = std::make_shared<hse::Primitive>(hse::PrimitiveType::Cube);
    found->setName("foundation_0"); found->setPosition({0, 0, 0}); found->setScale({10, 0.5f, 10});
    auto floor1 = std::make_shared<hse::Primitive>(hse::PrimitiveType::Cube);
    floor1->setName("floor_1"); floor1->setPosition({0, 0.5f, 0}); floor1->setScale({10, 0.2f, 10});
    auto wall = std::make_shared<hse::Primitive>(hse::PrimitiveType::Cube);
    wall->setName("wall_1"); wall->setPosition({0, 1.8f, 0}); wall->setScale({10, 2.4f, 0.2f});
    auto roof = std::make_shared<hse::Primitive>(hse::PrimitiveType::Cube);
    roof->setName("roof_main"); roof->setPosition({0, 3.2f, 0}); roof->setScale({10, 0.4f, 10});

    scene->addPrimitive(found);
    scene->addPrimitive(floor1);
    scene->addPrimitive(wall);
    scene->addPrimitive(roof);

    scene->addRelationship("foundation_0", "supports", "floor_1", 300.0f, 5.0f);
    scene->addRelationship("floor_1", "supports", "wall_1", 300.0f, 5.0f);
    scene->addRelationship("wall_1", "supports", "roof_main", 300.0f, 5.0f);

    // Initial steady state update
    scene->update(0.016f);

    // Dynamic physical disturbance: apply upward impulse to foundation
    found->setVelocity({0.0f, 5.0f, 0.0f});

    // Step simulation
    for (int step = 0; step < 10; ++step) {
        scene->update(0.016f);
    }

    // Proof: motion propagated to roof via spring coupling
    assert(roof->getVelocity().y > 0.1f || roof->getPosition().y > 3.2f);
    std::cout << "PASS 6: Coupled physical propagation verified (roof velocity Y=" << roof->getVelocity().y << ")" << std::endl;
}

// TEST 7: Energy Accounting
void test_7_energy_accounting() {
    auto scene = std::make_shared<hse::Scene>("Energy Accounting");
    auto p = std::make_shared<hse::Primitive>(hse::PrimitiveType::Cube);
    p->setPosition({0, 2.0f, 0});
    p->setRestPosition({0, 0.0f, 0}); // Displacement d = 2.0m
    p->setSelfStiffness(100.0f); p->setSelfDamping(0.0f);
    scene->addPrimitive(p);

    // Initial potential energy Ep = 0.5 * 100 * (2^2) = 200 J
    assert(approxEqual(scene->getTotalSystemEnergy(), 200.0f));

    // High precision sub-stepping for energy conservation validation
    for (int step = 0; step < 50; ++step) {
        scene->update(0.001f);
    }
    float eSystem = scene->getTotalSystemEnergy();
    std::cout << "DEBUG test_7: eSystem = " << eSystem << " J" << std::endl;
    // System energy remains conserved within tight numerical bounds
    assert(approxEqual(eSystem, 200.0f, 10.0f));
    std::cout << "PASS 7: System energy accounting verified (E=" << eSystem << "J)" << std::endl;
}

// TEST 8: Physical State Reaches Rendering
void test_8_rendering_reachability() {
    auto scene = std::make_shared<hse::Scene>("Rendering Reachability");
    auto p = std::make_shared<hse::Primitive>(hse::PrimitiveType::Cube);
    p->setPosition({0, 0, 0}); p->setMass(1.0f); p->setSelfStiffness(0.0f); p->setSelfDamping(0.0f);
    scene->addPrimitive(p);

    p->addForce({0, 50.0f, 0});
    scene->update(0.1f); // Dynamic physics moves position to Y=0.25m

    assert(p->getPosition().y > 0.20f);
    assert(p->getWorldPosition().y > 0.20f);
    assert(p->getWorldTransform().ptr()[15] == 1.0f); // Valid GL matrix
    std::cout << "PASS 8: Physical state reaches GL world transform (world Y=" << p->getWorldPosition().y << ")" << std::endl;
}

// TEST 9: Interactive Camera Controls Verification
void test_9_interactive_controls() {
    hse::Camera cam;
    cam.setPosition({0, 4, 15});
    cam.lookAt({0, 2.5f, 0});

    // WASDQE Keyboard Movement
    cam.processKeyboard("FORWARD", 0.1f, false);
    assert(cam.getPosition().z < 15.0f); // Moved forward

    cam.processKeyboard("UP", 0.1f, true); // Speed boost
    assert(cam.getPosition().y > 4.0f); // Moved up

    // Mouse Look (Yaw / Pitch)
    cam.processMouseMovement(100.0f, 50.0f); // Look right and up
    assert(cam.getYaw() > -90.0f);
    assert(cam.getPitch() > 0.0f);

    // Pitch Clamping (-89 to +89)
    cam.processMouseMovement(0.0f, 2000.0f);
    assert(cam.getPitch() == 89.0f);

    // Reset Camera Focus
    cam.resetFocus({0, 4, 15}, {0, 2.5f, 0});
    assert(cam.getPosition().y == 4.0f);
    assert(cam.getPitch() == 0.0f);
    std::cout << "PASS 9: Interactive WASDQE and mouse look camera controls verified" << std::endl;
}

int main() {
    test_1_physical_state_init();
    test_2_integration();
    test_3_stiffness_experiment();
    test_4_damping_experiment();
    test_5_mass_experiment();
    test_6_coupled_propagation();
    test_7_energy_accounting();
    test_8_rendering_reachability();
    test_9_interactive_controls();
    std::cout << "All Physical Harmonic Engine tests passed (9/9)!" << std::endl;
    return 0;
}
