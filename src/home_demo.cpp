#include "core/window.h"
#include "renderer/renderer.h"
#include "scene/scene_builder.h"
#include "scene/scene.h"
#include "scene/primitive.h"
#include "scene/light.h"
#include "scene/camera.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <chrono>

int main() {
    hse::Window window({"Two-Story Home — HarmonicStringGameEngine", 1280, 720, true});
    if (!window.isValid()) { std::cerr << "Failed to create window\n"; return -1; }
    hse::Renderer renderer;
    if (!renderer.initialize()) { std::cerr << "Failed renderer\n"; return -1; }

    auto scene = hse::SceneBuilder::buildTwoStoryHome();
    auto camera = scene->getActiveCamera();
    camera->setAspectRatio(static_cast<float>(window.getWidth())/window.getHeight());
    camera->update();

    // Register resize
    window.setResizeCallback([&](int w,int h){
        if (camera) { camera->setAspectRatio(static_cast<float>(w)/h); camera->update(); }
        renderer.setViewport(0,0,w,h);
    });

    // Camera presets: exterior, floor1, floor2, interior, harmonic demo
    struct Preset { const char* name; hse::Vec3 pos; hse::Vec3 target; float fov; };
    Preset presets[] = {
        {"Exterior Overview", {14,9,14}, {0,3,0}, 55},
        {"Floor 1 Interior", {0,1.6f,6.0f}, {0,1.2f,0}, 65},
        {"Floor 2 Interior", {0,4.6f,6.0f}, {0,4.2f,0}, 65},
        {"Side Section", {12,4,0}, {0,3,0}, 50},
        {"Harmonic Propagation Demo", {8,5,10}, {0,3,0}, 55},
    };
    int presetIdx = 0;
    auto applyPreset = [&](int idx){
        auto &p = presets[idx];
        camera->setPosition(p.pos);
        camera->lookAt(p.target);
        camera->setFOV(p.fov);
        camera->update();
        std::cout << "Camera: " << p.name << " pos " << p.pos.x << "," << p.pos.y << "," << p.pos.z << "\n";
    };
    applyPreset(0);

    // Headless/timed mode support
    bool headless = false;
    const char* h = std::getenv("HSE_HEADLESS");
    if (h && std::string(h)=="1") headless = true;

    renderer.setClearColor(0.08f, 0.09f, 0.12f);
    std::cout << "TwoStoryHome: " << scene->getPrimitiveCount() << " primitives, " << scene->getLightCount() << " lights\n";

    // Harmonic-like live demonstration: we will animate stairs harmonic via rotationSpeed on stair rail + propagate parent move
    auto stairs = scene->findByName("stairs");
    auto floor1 = scene->findByName("floor_1");
    auto houseRoot = scene->findByName("house_root");

    double startTime = glfwGetTime();
    double lastParamChange = startTime;
    bool harmonicToggled = false;
    int frameCount = 0;

    while (!window.shouldClose()) {
        float dt = window.getDeltaTime();
        // Cap dt for stability
        if (dt > 0.1f) dt = 0.1f;
        window.pollEvents();

        // Keys: 1-5 presets, ESC close, SPACE harmonic impulse (add light), H propagate demo
        // ESC via window check isHandled via shouldClose polling internally
        if (window.isKeyPressed(49)) { if (presetIdx!=0){presetIdx=0; applyPreset(0);} }
        if (window.isKeyPressed(50)) { if (presetIdx!=1){presetIdx=1; applyPreset(1);} }
        if (window.isKeyPressed(51)) { if (presetIdx!=2){presetIdx=2; applyPreset(2);} }
        if (window.isKeyPressed(52)) { if (presetIdx!=3){presetIdx=3; applyPreset(3);} }
        if (window.isKeyPressed(53)) { if (presetIdx!=4){presetIdx=4; applyPreset(4);} }
        if (window.isKeyPressed(32)) { // SPACE harmonic impulse
            static double last=0;
            double now=glfwGetTime();
            if (now-last>0.5){
                last=now;
                auto impulse = std::make_shared<hse::Light>("impulse");
                impulse->setPosition({0,4,0});
                impulse->setColor({1,0.9f,0.6f});
                impulse->setIntensity(1.5f);
                scene->addLight(impulse);
                std::cout << "Harmonic impulse: lights " << scene->getLightCount() << "\n";
            }
        }
        if (window.isKeyPressed(72)) { // H propagate structural parameter
            static double lastH=0;
            double now=glfwGetTime();
            if (now-lastH>0.6){
                lastH=now;
                // Move floor_1 slightly — all its descendants propagate via hierarchy (PROVEN)
                hse::Vec3 pos = floor1->getPosition();
                pos.x += 0.4f;
                if (pos.x > 1.2f) pos.x = 0.0f;
                floor1->setPosition(pos);
                // Force world recalc visible via renderer's computeAllWorldMatrices next frame
                std::cout << "Propagation demo: floor_1 x -> " << pos.x << " ; wall_f1_north world will follow\n";
                // Demonstrate house-level harmonic structural shift
                if (stairs) {
                    hse::Vec3 r = stairs->getRotation();
                    r.y += 2.0f;
                    stairs->setRotation(r);
                }
            }
        }

        // Automatic harmonic demo in headless: oscillate houseRoot slightly every 1.5s
        double now = glfwGetTime();
        if (headless && now - lastParamChange > 1.5) {
            lastParamChange = now;
            harmonicToggled = !harmonicToggled;
            if (floor1) {
                hse::Vec3 p = floor1->getPosition();
                p.x = harmonicToggled ? 0.5f : 0.0f;
                floor1->setPosition(p);
            }
        }

        scene->update(dt);
        // Renderer computes world matrices
        renderer.beginFrame();
        renderer.renderScene(*scene, *camera);
        renderer.endFrame();
        window.swapBuffers();

        frameCount++;
        if (headless && now - startTime > 4.0) {
            std::cout << "Headless run complete: " << frameCount << " frames\n";
            break;
        }
    }

    renderer.shutdown();
    return 0;
}
