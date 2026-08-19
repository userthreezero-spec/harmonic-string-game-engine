#include "core/window.h"
#include "renderer/renderer.h"
#include "scene/scene.h"
#include "scene/camera.h"
#include "scene/primitive.h"
#include "scene/scene_builder.h"
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

    auto scene = hse::SceneBuilder::buildRoom();
    auto camera = scene->getActiveCamera();
    if (!camera) {
        std::cerr << "No camera in scene" << std::endl;
        return -1;
    }
    camera->setAspectRatio(static_cast<float>(window.getWidth()) / window.getHeight());

    camera->setOrbitRadius(8.0f);
    camera->setOrbitSpeed(0.5f);
    camera->enableOrbit(true);

    renderer.setClearColor(0.05f, 0.05f, 0.08f);

    std::cout << "HSE-015 Camera Orbit Demo" << std::endl;
    std::cout << "Radius: 8.0, Speed: 0.5 rad/s" << std::endl;
    std::cout << "Press ESC to exit" << std::endl;

    int frameCount = 0;
    while (!window.shouldClose() && frameCount < 300) {
        float dt = window.getDeltaTime();
        window.pollEvents();

        scene->update(dt);

        renderer.beginFrame();
        renderer.renderScene(*scene, *camera);
        renderer.endFrame();

        window.swapBuffers();
        frameCount++;
    }

    renderer.shutdown();
    std::cout << "Orbit demo completed after " << frameCount << " frames" << std::endl;
    return 0;
}
