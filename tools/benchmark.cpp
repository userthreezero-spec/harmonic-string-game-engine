#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "renderer/renderer.h"
#include "scene/scene.h"
#include "scene/camera.h"
#include "scene/primitive.h"
#include "renderer/material.h"
#include "math/vec3.h"
#include <cstdio>
#include <chrono>
#include <memory>
#include <vector>

using namespace hse;

int main() {
    if (!glfwInit()) return 1;
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* win = glfwCreateWindow(800, 600, "benchmark", nullptr, nullptr);
    if (!win) return 1;
    glfwMakeContextCurrent(win);

    Renderer renderer;
    if (!renderer.initialize()) return 1;

    auto scene = std::make_shared<Scene>("Bench");
    auto camera = std::make_shared<Camera>(ProjectionType::Perspective);
    camera->setPosition({0, 30, 60});
    camera->lookAt({0, 0, 0});
    camera->update();

    const int count = 2000;
    for (int i = 0; i < count; ++i) {
        auto p = std::make_shared<Primitive>(PrimitiveType::Cube);
        p->setPosition({(float)(i % 20) * 2.5f - 20.0f, (float)((i / 20) % 10) * 2.5f - 10.0f, (float)(i / 200) * 2.5f - 10.0f});
        p->uploadGPU();
        scene->addPrimitive(p);
    }

    const int frames = 100;
    double totalUpdate = 0, totalMatrix = 0, totalRender = 0;

    // Warmup
    for (int i = 0; i < 5; ++i) {
        renderer.beginFrame();
        renderer.renderScene(*scene, *camera);
        renderer.endFrame();
        glFinish();
    }

    for (int i = 0; i < frames; ++i) {
        // 1. Update (Simulation)
        auto t0 = std::chrono::high_resolution_clock::now();
        scene->update(0.016f);
        auto t1 = std::chrono::high_resolution_clock::now();

        // 2. Matrix Computation (Explicitly call to measure)
        // Renderer also calls it, so this measures the cost once.
        auto tm0 = std::chrono::high_resolution_clock::now();
        scene->computeAllWorldMatrices();
        auto tm1 = std::chrono::high_resolution_clock::now();

        // 3. Render
        auto tr0 = std::chrono::high_resolution_clock::now();
        renderer.beginFrame();
        renderer.renderScene(*scene, *camera);
        renderer.endFrame();
        glFinish();
        auto tr1 = std::chrono::high_resolution_clock::now();

        totalUpdate += std::chrono::duration<double>(t1 - t0).count();
        totalMatrix += std::chrono::duration<double>(tm1 - tm0).count();
        totalRender += std::chrono::duration<double>(tr1 - tr0).count();
    }

    printf("BASELINE_METRICS (Phase 1140)\n");
    printf("Cubes: %d\n", count);
    printf("Avg Update: %f ms\n", (totalUpdate / frames) * 1000.0);
    printf("Avg Matrix: %f ms\n", (totalMatrix / frames) * 1000.0);
    printf("Avg Render+Wait: %f ms\n", (totalRender / frames) * 1000.0);
    printf("Avg Total: %f ms\n", ((totalUpdate + totalMatrix + totalRender) / frames) * 1000.0);
    printf("FPS: %f\n", frames / (totalUpdate + totalMatrix + totalRender));

    glfwTerminate();
    return 0;
}
