#include "core/window.h"
#include "renderer/renderer.h"
#include "scene/scene.h"
#include "scene/camera.h"
#include "scene/primitive.h"
#include "scene/scene_builder.h"
#include "bridge/bridge.h"
#include "scene/picker.h"
#include <GLFW/glfw3.h>
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

    uint64_t selectedID = 0;
    double prevMouseX = 0, prevMouseY = 0;
    bool isDragging = false;
    bool isPanning = false;

    // Main loop
    while (!window.shouldClose()) {
        float dt = window.getDeltaTime();
        window.pollEvents();

        // Update renderer viewport and camera aspect ratio
        int width = window.getWidth();
        int height = window.getHeight();
        renderer.setViewport(0, 0, width, height);
        if (height > 0) {
            camera->setAspectRatio(static_cast<float>(width) / height);
        }

        // Handle Mouse Navigation
        double mx, my;
        window.getMousePosition(mx, my);
        bool leftPressed = window.isMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT);
        bool middlePressed = window.isMouseButtonPressed(GLFW_MOUSE_BUTTON_MIDDLE);
        bool shiftPressed = window.isKeyPressed(GLFW_KEY_LEFT_SHIFT) || window.isKeyPressed(GLFW_KEY_RIGHT_SHIFT);

        if (leftPressed || middlePressed) {
            if (!isDragging && !isPanning) {
                prevMouseX = mx;
                prevMouseY = my;
                if (leftPressed && !shiftPressed) isDragging = true;
                else isPanning = true;
            }

            double dx = mx - prevMouseX;
            double dy = my - prevMouseY;
            prevMouseX = mx;
            prevMouseY = my;

            if (isDragging) {
                camera->offsetOrbit(static_cast<float>(dx) * 0.2f, static_cast<float>(-dy) * 0.2f);
            } else if (isPanning) {
                camera->pan(static_cast<float>(-dx) * 0.01f, static_cast<float>(dy) * 0.01f);
            }
        } else {
            // Check for click (selection)
            if (isDragging || isPanning) {
                // Was dragging, now released
            } else {
                // Optional: handle single click here if needed, but better to check release
            }
            isDragging = false;
            isPanning = false;
        }

        // Mouse Picking on Left Click (release)
        static bool lastLeftPressed = false;
        if (!leftPressed && lastLeftPressed && !shiftPressed) {
            // Check if it was a tiny movement (click) or a drag
            // For simplicity, always try to pick on release if not dragging
            hse::Ray ray = hse::Picker::screenToRay(mx, my, *camera, window.getWidth(), window.getHeight());
            auto hit = hse::Picker::pick(ray, *scene);
            if (hit.hit) {
                selectedID = hit.objectID;
                auto obj = scene->findByID(selectedID);
                std::cout << "Selected: " << (obj ? obj->getName() : "Unknown") << " (ID: " << selectedID << ")" << std::endl;
            } else {
                selectedID = 0;
            }
        }
        lastLeftPressed = leftPressed;

        // Zoom
        float scrollY = window.getScrollY();
        if (std::abs(scrollY) > 0.01f) {
            camera->zoom(scrollY * 0.5f);
            window.resetScroll();
        }

        // Manipulation
        if (selectedID != 0) {
            auto obj = scene->findByID(selectedID);
            if (obj) {
                hse::Vec3 pos = obj->getPosition();
                hse::Vec3 rot = obj->getRotation();
                hse::Vec3 scl = obj->getScale();
                float speed = 2.0f * dt;

                if (window.isKeyPressed(GLFW_KEY_UP))    pos.z -= speed;
                if (window.isKeyPressed(GLFW_KEY_DOWN))  pos.z += speed;
                if (window.isKeyPressed(GLFW_KEY_LEFT))  pos.x -= speed;
                if (window.isKeyPressed(GLFW_KEY_RIGHT)) pos.x += speed;
                if (window.isKeyPressed(GLFW_KEY_PAGE_UP))   pos.y += speed;
                if (window.isKeyPressed(GLFW_KEY_PAGE_DOWN)) pos.y -= speed;

                if (window.isKeyPressed(GLFW_KEY_HOME)) rot.y += speed * 20.0f;
                if (window.isKeyPressed(GLFW_KEY_END))  rot.y -= speed * 20.0f;

                if (window.isKeyPressed(GLFW_KEY_EQUAL)) scl += {0.1f * speed, 0.1f * speed, 0.1f * speed};
                if (window.isKeyPressed(GLFW_KEY_MINUS)) scl -= {0.1f * speed, 0.1f * speed, 0.1f * speed};

                obj->setPosition(pos);
                obj->setRotation(rot);
                obj->setScale(scl);
            }
        }

        // Save
        static bool lastSPressed = false;
        bool sPressed = window.isKeyPressed(GLFW_KEY_S);
        if (sPressed && !lastSPressed) {
            if (!manifest.filePath.empty()) {
                std::cout << "Saving project to: " << manifest.filePath << std::endl;
                hse::SceneBuilder::exportHSC(*scene, *camera, manifest, manifest.filePath);
            }
        }
        lastSPressed = sPressed;

        bridge.pumpCommands(scene, camera, renderer);

        scene->update(dt);

        renderer.beginFrame();
        renderer.renderScene(*scene, *camera, selectedID);
        renderer.endFrame();
        renderer.incrementFrameCount();
        window.swapBuffers();
    }

    // Shutdown
    bridge.stop();
    renderer.shutdown();
    return 0;
}
