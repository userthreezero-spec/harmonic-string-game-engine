#include "core/window.h"
#include "renderer/renderer.h"
#include "renderer/material.h"
#include "scene/scene.h"
#include "scene/camera.h"
#include "scene/primitive.h"
#include <iostream>
#include <memory>

int main() {
    hse::Window window;
    if (!window.isValid()) {
        std::cerr << "Failed to create window" << std::endl;
        return -1;
    }

    hse::Renderer renderer;
    if (!renderer.initialize()) {
        std::cerr << "Failed to initialize renderer" << std::endl;
        return -1;
    }

    auto scene = std::make_shared<hse::Scene>("Main Scene");

    auto camera = std::make_shared<hse::Camera>(hse::ProjectionType::Perspective);
    camera->setPosition({0.0f, 0.0f, 5.0f});
    camera->lookAt({0.0f, 0.0f, 0.0f});
    camera->setAspectRatio(static_cast<float>(window.getWidth()) / window.getHeight());
    camera->update();
    scene->addCamera(camera);

    auto matGreen = std::make_shared<hse::Material>("Green");
    matGreen->setAlbedo({0.2f, 0.8f, 0.3f});
    scene->addMaterial(matGreen);

    auto triangle = std::make_shared<hse::Primitive>(hse::PrimitiveType::Triangle);
    triangle->setPosition({0.0f, 0.0f, 0.0f});
    triangle->setMaterial(matGreen);
    triangle->setRotationSpeed({0.0f, 1.0f, 0.0f});
    triangle->uploadGPU();
    scene->addPrimitive(triangle);

    renderer.setClearColor(0.1f, 0.1f, 0.15f);

    std::cout << "Harmonic String Engine v0.1 - Material System Foundation" << std::endl;
    std::cout << "Press ESC to exit" << std::endl;

    while (!window.shouldClose()) {
        float dt = window.getDeltaTime();
        window.pollEvents();

        scene->update(dt);

        renderer.beginFrame();
        renderer.renderScene(*scene, *camera);
        renderer.endFrame();

        window.swapBuffers();
    }

    renderer.shutdown();
    return 0;
}
