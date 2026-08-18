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
    camera->update();

    hse::SceneBuilder::exportState(*scene, "C:/Users/Rico/HarmonicStringEngine/assets/worlds/scene_state.json");

    renderer.setClearColor(0.05f, 0.05f, 0.08f);

    std::cout << "HSE-003 Room Demo - " << scene->getPrimitiveCount() << " primitives" << std::endl;
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
