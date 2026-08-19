#include "core/window.h"
#include "renderer/renderer.h"
#include "renderer/texture.h"
#include "renderer/material.h"
#include "scene/scene.h"
#include "scene/camera.h"
#include "scene/primitive.h"
#include <iostream>
#include <memory>

int main() {
    hse::Window window;
    if (!window.isValid()) return -1;

    hse::Renderer renderer;
    if (!renderer.initialize()) return -1;

    auto scene = std::make_shared<hse::Scene>("Material Demo");

    auto camera = std::make_shared<hse::Camera>(hse::ProjectionType::Perspective);
    camera->setPosition({0.0f, 2.0f, 6.0f});
    camera->lookAt({0.0f, 0.0f, 0.0f});
    camera->setAspectRatio(static_cast<float>(window.getWidth()) / window.getHeight());
    camera->update();
    scene->addCamera(camera);

    auto logoTex = std::make_shared<hse::Texture>("assets/textures/logo.png");

    // Material 1: Solid Red
    auto matRed = std::make_shared<hse::Material>("Red");
    matRed->setAlbedo({1.0f, 0.2f, 0.2f});
    matRed->setRoughness(0.2f);
    scene->addMaterial(matRed);

    // Material 2: Textured
    auto matTextured = std::make_shared<hse::Material>("Logo");
    matTextured->setAlbedo({1.0f, 1.0f, 1.0f});
    matTextured->setAlbedoMap(logoTex);
    scene->addMaterial(matTextured);

    // Material 3: Metallic Gold-ish
    auto matGold = std::make_shared<hse::Material>("Gold");
    matGold->setAlbedo({1.0f, 0.8f, 0.0f});
    matGold->setMetallic(1.0f);
    matGold->setRoughness(0.1f);
    scene->addMaterial(matGold);

    // Primitive A: Red Triangle
    auto tri = std::make_shared<hse::Primitive>(hse::PrimitiveType::Triangle);
    tri->setPosition({-2.0f, 0.0f, 0.0f});
    tri->setMaterial(matRed);
    tri->uploadGPU();
    scene->addPrimitive(tri);

    // Primitive B: Textured Quad
    auto quad = std::make_shared<hse::Primitive>(hse::PrimitiveType::Quad);
    quad->setPosition({0.0f, 0.0f, 0.0f});
    quad->setMaterial(matTextured);
    quad->uploadGPU();
    scene->addPrimitive(quad);

    // Primitive C: Metallic Cube
    auto cube = std::make_shared<hse::Primitive>(hse::PrimitiveType::Cube);
    cube->setPosition({2.0f, 0.0f, 0.0f});
    cube->setMaterial(matGold);
    cube->uploadGPU();
    scene->addPrimitive(cube);

    // Primitive D: Another Quad sharing "Red" material
    auto quad2 = std::make_shared<hse::Primitive>(hse::PrimitiveType::Quad);
    quad2->setPosition({-2.0f, 1.5f, 0.0f});
    quad2->setMaterial(matRed);
    quad2->uploadGPU();
    scene->addPrimitive(quad2);

    renderer.setClearColor(0.15f, 0.15f, 0.2f);

    std::cout << "HSE Material System Demo" << std::endl;
    std::cout << "Prims: Red Triangle, Logo Quad, Gold Cube, Red Quad (Shared Mat)" << std::endl;

    int frameCount = 0;
    while (!window.shouldClose() && frameCount < 100) {
        float dt = window.getDeltaTime();
        window.pollEvents();

        tri->setRotation({0.0f, (float)frameCount * 0.02f, 0.0f});
        cube->setRotation({(float)frameCount * 0.01f, (float)frameCount * 0.015f, 0.0f});

        scene->update(dt);

        renderer.beginFrame();
        renderer.renderScene(*scene, *camera);
        renderer.endFrame();

        window.swapBuffers();
        frameCount++;
    }

    renderer.shutdown();
    std::cout << "Material demo completed" << std::endl;
    return 0;
}
