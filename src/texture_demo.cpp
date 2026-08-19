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
    if (!window.isValid()) {
        std::cerr << "Failed to create window" << std::endl;
        return -1;
    }

    hse::Renderer renderer;
    if (!renderer.initialize()) {
        std::cerr << "Failed to initialize renderer" << std::endl;
        return -1;
    }

    auto scene = std::make_shared<hse::Scene>("Texture Demo Scene");

    auto camera = std::make_shared<hse::Camera>(hse::ProjectionType::Perspective);
    camera->setPosition({0.0f, 0.0f, 3.0f});
    camera->lookAt({0.0f, 0.0f, 0.0f});
    camera->setAspectRatio(static_cast<float>(window.getWidth()) / window.getHeight());
    camera->update();
    scene->addCamera(camera);

    // Create a textured quad
    auto quad = std::make_shared<hse::Primitive>(hse::PrimitiveType::Quad);
    quad->setPosition({0.0f, 0.0f, 0.0f});

    auto texture = std::make_shared<hse::Texture>("assets/textures/logo.png");
    auto quadMat = std::make_shared<hse::Material>("textured_quad");
    quadMat->setAlbedo({1.0f, 1.0f, 1.0f});
    quadMat->setAlbedoMap(texture);
    quad->setMaterial(quadMat);
    scene->addMaterial(quadMat);

    quad->uploadGPU();
    scene->addPrimitive(quad);

    renderer.setClearColor(0.2f, 0.2f, 0.25f);

    std::cout << "HSE Texture Demo - Rendering with stb_image" << std::endl;

    // Run for a few frames then exit (for automated check if possible, or just proof of build)
    int frameCount = 0;
    while (!window.shouldClose() && frameCount < 100) {
        float dt = window.getDeltaTime();
        window.pollEvents();

        quad->setRotation({0.0f, 0.0f, (float)frameCount * 0.01f});
        scene->update(dt);

        renderer.beginFrame();
        renderer.renderScene(*scene, *camera);
        renderer.endFrame();

        window.swapBuffers();
        frameCount++;
    }

    renderer.shutdown();
    std::cout << "Demo completed successfully" << std::endl;
    return 0;
}
