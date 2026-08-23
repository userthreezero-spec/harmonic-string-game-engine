// HSE headless multi-view capture tool (WebOS Phase 1046 perception interface).
//
// Renders the HSE demo scene to an OFFSCREEN framebuffer (FBO) from several camera
// viewpoints and writes each frame as a binary PPM, plus a manifest.json describing the
// camera poses and the scene ground truth. Using an FBO (not the window framebuffer) makes
// glReadPixels return valid pixels regardless of window visibility/occlusion, so capture is
// deterministic and headless-safe.
//
// Usage: hse_capture <output_dir>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "renderer/renderer.h"
#include "scene/scene.h"
#include "scene/camera.h"
#include "scene/primitive.h"
#include "renderer/material.h"
#include "math/vec3.h"
#include <cstdio>
#include <fstream>
#include <memory>
#include <string>
#include <vector>

using namespace hse;

static const int CW = 800, CH = 600;

struct Obj { const char* name; int type; float x, y, z; float sx, sy, sz; float r, g, b; };
// A known world of REAL cube geometry (the Sphere primitive is a stub): a wide red slab
// (left), a tall green box (center), a blue cube (right). Distinct colors + shapes + places.
static const int CUBE = 2;
static const std::vector<Obj> WORLD = {
    {"red_cube",  CUBE, -3.5f, 0.0f, 0.0f, 2.0f, 2.0f, 2.0f, 0.90f, 0.12f, 0.12f},
    {"green_cube",CUBE,  0.0f, 0.0f, 2.0f, 2.0f, 2.0f, 2.0f, 0.12f, 0.85f, 0.20f},
    {"blue_cube", CUBE,  3.5f, 0.0f, 0.0f, 2.0f, 2.0f, 2.0f, 0.15f, 0.30f, 0.92f},
};
// General-position viewpoints (axis-aligned cameras exhibit an HSE view/culling quirk that
// can drop on-axis objects); these three orbit the scene and render all objects reliably,
// placing them at distinct screen positions so multi-view perception is meaningful.
struct View { const char* name; float px, py, pz; };
static const std::vector<View> VIEWS = {
    {"orbit_left",  -6.0f, 5.0f, 7.0f},
    {"orbit_right",  6.0f, 5.0f, 7.0f},
    {"orbit_high",   2.0f, 8.0f, 6.0f},
};

static void writePPM(const std::string& path, const std::vector<unsigned char>& rgba, int w, int h) {
    std::ofstream f(path, std::ios::binary);
    f << "P6\n" << w << " " << h << "\n255\n";
    // FBO/glReadPixels is bottom-up; flip vertically to top-down PPM.
    for (int y = h - 1; y >= 0; --y) {
        const unsigned char* row = &rgba[(size_t)y * w * 4];
        for (int x = 0; x < w; ++x) {
            f.put((char)row[x * 4 + 0]); f.put((char)row[x * 4 + 1]); f.put((char)row[x * 4 + 2]);
        }
    }
}

int main(int argc, char** argv) {
    if (argc < 2) { std::fprintf(stderr, "usage: hse_capture <output_dir>\n"); return 2; }
    std::string outDir = argv[1];

    if (!glfwInit()) { std::fprintf(stderr, "glfwInit failed\n"); return 1; }
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);      // headless: no visible window
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    GLFWwindow* win = glfwCreateWindow(CW, CH, "hse_capture", nullptr, nullptr);
    if (!win) { std::fprintf(stderr, "createWindow failed\n"); glfwTerminate(); return 1; }
    glfwMakeContextCurrent(win);

    Renderer renderer;
    if (!renderer.initialize()) { std::fprintf(stderr, "renderer init failed\n"); return 1; }
    renderer.setClearColor(0.05f, 0.05f, 0.08f);

    // Offscreen framebuffer (color texture + depth renderbuffer).
    GLuint fbo = 0, colorTex = 0, depthRb = 0;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glGenTextures(1, &colorTex);
    glBindTexture(GL_TEXTURE_2D, colorTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, CW, CH, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTex, 0);
    glGenRenderbuffers(1, &depthRb);
    glBindRenderbuffer(GL_RENDERBUFFER, depthRb);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, CW, CH);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRb);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::fprintf(stderr, "FBO incomplete\n"); return 1;
    }

    // Build the known world.
    auto scene = std::make_shared<Scene>("CaptureScene");
    for (const auto& o : WORLD) {
        auto prim = std::make_shared<Primitive>(static_cast<PrimitiveType>(o.type));
        prim->setPosition({o.x, o.y, o.z});
        prim->setScale({o.sx, o.sy, o.sz});
        auto mat = std::make_shared<Material>(o.name);
        mat->setAlbedo({o.r, o.g, o.b});
        prim->setMaterial(mat);
        prim->setName(o.name);
        prim->uploadGPU();
        scene->addMaterial(mat);
        scene->addPrimitive(prim);
    }

    std::ofstream man(outDir + "/manifest.json");
    man << "{\"width\":" << CW << ",\"height\":" << CH << ",\"scene\":\"CaptureScene\",";
    man << "\"ground_truth_objects\":[";
    for (size_t i = 0; i < WORLD.size(); ++i) {
        const auto& o = WORLD[i];
        man << "{\"name\":\"" << o.name << "\",\"type\":" << o.type
            << ",\"pos\":[" << o.x << "," << o.y << "," << o.z << "]"
            << ",\"albedo\":[" << o.r << "," << o.g << "," << o.b << "]}"
            << (i + 1 < WORLD.size() ? "," : "");
    }
    man << "],\"views\":[";

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glViewport(0, 0, CW, CH);
    std::vector<unsigned char> rgba((size_t)CW * CH * 4);
    for (size_t v = 0; v < VIEWS.size(); ++v) {
        const auto& view = VIEWS[v];
        auto cam = std::make_shared<Camera>(ProjectionType::Perspective);
        cam->setPosition({view.px, view.py, view.pz});
        cam->lookAt({0.0f, 0.0f, 0.0f});
        cam->setAspectRatio((float)CW / (float)CH);
        cam->setFOV(55.0f);
        cam->update();
        // render a couple of frames to be safe, then read the FBO
        for (int f = 0; f < 2; ++f) { renderer.beginFrame(); renderer.renderScene(*scene, *cam); renderer.endFrame(); }
        glFinish();
        glReadPixels(0, 0, CW, CH, GL_RGBA, GL_UNSIGNED_BYTE, rgba.data());
        std::string fname = std::string("view_") + view.name + ".ppm";
        writePPM(outDir + "/" + fname, rgba, CW, CH);
        man << "{\"name\":\"" << view.name << "\",\"file\":\"" << fname << "\","
            << "\"camera_pos\":[" << view.px << "," << view.py << "," << view.pz << "]}"
            << (v + 1 < VIEWS.size() ? "," : "");
    }
    man << "]}";
    man.close();

    glfwDestroyWindow(win);
    glfwTerminate();
    std::printf("hse_capture: wrote %zu views to %s\n", VIEWS.size(), outDir.c_str());
    return 0;
}
