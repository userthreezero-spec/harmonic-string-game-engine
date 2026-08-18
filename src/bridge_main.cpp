#include "core/window.h"
#include "renderer/renderer.h"
#include "scene/scene.h"
#include "scene/camera.h"
#include "scene/primitive.h"
#include "bridge/bridge.h"
#include <iostream>
#include <memory>

int main(int argc, char* argv[]) {
    std::cout << "=== HSE Live Bridge ===" << std::endl;
    std::cout << "Pipe: \\\\.\\pipe\\hse_bridge" << std::endl;
    std::cout << "ESC to quit" << std::endl;
    std::cout << std::endl;

    hse::Window window(hse::WindowProps{"HSE Live Bridge", 1280, 720, true});
    if (!window.isValid()) {
        std::cerr << "Failed to create window" << std::endl;
        return 1;
    }

    hse::Renderer renderer;
    if (!renderer.initialize()) {
        std::cerr << "Failed to initialize renderer" << std::endl;
        return 1;
    }

    auto scene = std::make_shared<hse::Scene>("BridgeScene");
    auto camera = std::make_shared<hse::Camera>();
    camera->setPosition({5.0f, 5.0f, 5.0f});
    camera->lookAt({0.0f, 0.0f, 0.0f});
    camera->update();
    scene->addCamera(camera);

    // Start bridge
    hse::Bridge bridge("\\\\.\\pipe\\hse_bridge");
    if (!bridge.start()) {
        std::cerr << "Failed to start bridge" << std::endl;
        return 1;
    }

    renderer.setClearColor(0.05f, 0.05f, 0.08f);

    std::cout << "Waiting for external client..." << std::endl;
    std::cout << "Run: .\\phase005_probe\\controller.ps1" << std::endl;
    std::cout << std::endl;

    while (!window.shouldClose()) {
        float dt = window.getDeltaTime();
        window.pollEvents();

        bridge.pumpCommands(scene, camera, renderer);

        scene->update(dt);

        renderer.beginFrame();
        renderer.renderScene(*scene, *camera);
        renderer.endFrame();
        renderer.incrementFrameCount();
        window.swapBuffers();
    }

    bridge.stop();
    renderer.shutdown();
    return 0;
}
