#include "core/window.h"
#include "renderer/renderer.h"
#include "scene/scene.h"
#include "scene/camera.h"
#include "scene/primitive.h"
#include "scene/scene_builder.h"
#include "bridge/bridge.h"
#include <iostream>
#include <memory>
#include <string>

int main(int argc, char* argv[]) {
    std::string projectPath;
    std::string pipeName = "\\\\.\\pipe\\hse_bridge";

    // Parse arguments
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if ((arg == "--project" || arg == "-p") && i + 1 < argc) {
            projectPath = argv[++i];
        } else if ((arg == "--pipe") && i + 1 < argc) {
            pipeName = argv[++i];
        } else if (arg == "--help" || arg == "-h") {
            std::cout << "HSE Workspace Launcher" << std::endl;
            std::cout << "Usage: hse_workspace_app [options]" << std::endl;
            std::cout << "  --project <path>   Open .hsc project file" << std::endl;
            std::cout << "  --pipe <name>      Named pipe (default: \\\\.\\pipe\\hse_bridge)" << std::endl;
            std::cout << "  --help             Show this help" << std::endl;
            return 0;
        } else {
            // Treat as project path
            projectPath = arg;
        }
    }

    std::cout << "=== HSE Workspace ===" << std::endl;

    // Load project or create default
    hse::ProjectManifest manifest;
    std::shared_ptr<hse::Scene> scene;
    hse::WindowProps wp;

    if (!projectPath.empty()) {
        std::cout << "Opening project: " << projectPath << std::endl;
        scene = hse::SceneBuilder::importState(projectPath, manifest);
        if (!scene) {
            std::cerr << "Failed to load project: " << projectPath << std::endl;
            return 1;
        }
        wp.title = "HSE - " + manifest.projectName;
    } else {
        std::cout << "No project specified, using default scene" << std::endl;
        manifest.projectName = "DefaultScene";
        manifest.sceneName = "DefaultScene";
        manifest.clearColorR = 0.05f;
        manifest.clearColorG = 0.05f;
        manifest.clearColorB = 0.08f;
        scene = hse::SceneBuilder::buildRoom();
        wp.title = "HSE Workspace";
    }

    // Create window
    hse::Window window(wp);
    if (!window.isValid()) {
        std::cerr << "Failed to create window" << std::endl;
        return 1;
    }

    // Initialize renderer
    hse::Renderer renderer;
    if (!renderer.initialize()) {
        std::cerr << "Failed to initialize renderer" << std::endl;
        return 1;
    }

    renderer.setClearColor(manifest.clearColorR, manifest.clearColorG, manifest.clearColorB);

    // Ensure scene has a camera
    if (scene->getCameraCount() == 0) {
        auto camera = std::make_shared<hse::Camera>();
        camera->setPosition({5.0f, 5.0f, 5.0f});
        camera->lookAt({0.0f, 0.0f, 0.0f});
        camera->update();
        scene->addCamera(camera);
    }

    auto camera = scene->getActiveCamera();

    // Upload all GPU resources on GL thread
    for (auto& prim : scene->getPrimitives()) {
        prim->uploadGPU();
    }

    // Start bridge
    hse::Bridge bridge(pipeName);
    if (!bridge.start()) {
        std::cerr << "Failed to start bridge" << std::endl;
        return 1;
    }

    // Store project path in manifest for save
    manifest.filePath = projectPath;

    std::cout << "Pipe: " << pipeName << std::endl;
    std::cout << "Objects: " << scene->getPrimitiveCount() << std::endl;
    std::cout << "Camera: (" << camera->getPosition().x << ", " << camera->getPosition().y << ", " << camera->getPosition().z << ")" << std::endl;
    std::cout << std::endl;
    std::cout << "Waiting for external client..." << std::endl;
    std::cout << "Run: .\\phase006_probe\\controller.ps1" << std::endl;
    std::cout << std::endl;

    // Main loop
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

    // Shutdown
    bridge.stop();
    renderer.shutdown();
    return 0;
}
