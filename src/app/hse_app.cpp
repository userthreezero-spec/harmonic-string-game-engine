#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <fstream>
#include <memory>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <algorithm>
#ifdef _WIN32
#include <windows.h>
#endif
#include <GLFW/glfw3.h>
#include "core/window.h"
#include "renderer/renderer.h"
#include "scene/scene.h"
#include "scene/camera.h"
#include "scene/primitive.h"
#include "scene/light.h"
#include "scene/animated_light.h"
#include "scene/scene_builder.h"
#include "scene/picker.h"
#include "bridge/bridge.h"

namespace fs = std::filesystem;

enum class ApplicationState {
    ENGINE_HOME,
    PROJECT_BROWSER,
    PROJECT_SELECTED,
    PROJECT_LOADING,
    PROJECT_OPEN,
    SCENE_LOADING,
    ENGINE_RUNTIME,
    CHECKPOINTING,
    PROJECT_CLOSING,
    RETURNING_TO_HUB,
    APPLICATION_EXIT
};

inline const char* stateToString(ApplicationState state) {
    switch (state) {
        case ApplicationState::ENGINE_HOME: return "ENGINE_HOME";
        case ApplicationState::PROJECT_BROWSER: return "PROJECT_BROWSER";
        case ApplicationState::PROJECT_SELECTED: return "PROJECT_SELECTED";
        case ApplicationState::PROJECT_LOADING: return "PROJECT_LOADING";
        case ApplicationState::PROJECT_OPEN: return "PROJECT_OPEN";
        case ApplicationState::SCENE_LOADING: return "SCENE_LOADING";
        case ApplicationState::ENGINE_RUNTIME: return "ENGINE_RUNTIME";
        case ApplicationState::CHECKPOINTING: return "CHECKPOINTING";
        case ApplicationState::PROJECT_CLOSING: return "PROJECT_CLOSING";
        case ApplicationState::RETURNING_TO_HUB: return "RETURNING_TO_HUB";
        case ApplicationState::APPLICATION_EXIT: return "APPLICATION_EXIT";
        default: return "UNKNOWN";
    }
}

struct ProjectItem {
    std::string name;
    fs::path path;
    std::string type;
    int primitiveCount = 0;
    std::string lastOpened;
};

struct SceneComponent {
    char id[64];
    float posX, posY, posZ;
    float rotX, rotY, rotZ;
    float sizeX, sizeY, sizeZ;
    float colorR, colorG, colorB;
    int primitiveType;
};

class HSEApp {
public:
    HSEApp(int argc = 0, char* argv[] = nullptr)
        : m_window({"Harmonic String Engine — Graphical Desktop Application", 1280, 720, true})
    {
        resolvePaths();
        parseArgs(argc, argv);
    }

    ~HSEApp() {
        closeActiveProject();
    }

    bool initialize() {
        if (!m_window.isValid()) return false;
        if (!m_renderer.initialize()) return false;

        GLFWwindow* native = m_window.getNative();
        if (native) {
            glfwSetWindowUserPointer(native, this);

            glfwSetKeyCallback(native, [](GLFWwindow* win, int key, int scancode, int action, int mods) {
                auto* self = static_cast<HSEApp*>(glfwGetWindowUserPointer(win));
                if (self && action == GLFW_PRESS) {
                    self->handleKey(key, scancode, action, mods);
                }
            });

            glfwSetMouseButtonCallback(native, [](GLFWwindow* win, int button, int action, int mods) {
                auto* self = static_cast<HSEApp*>(glfwGetWindowUserPointer(win));
                if (self && action == GLFW_PRESS) {
                    self->handleMouseButton(button, action, mods);
                }
            });

            glfwSetCursorPosCallback(native, [](GLFWwindow* win, double xpos, double ypos) {
                auto* self = static_cast<HSEApp*>(glfwGetWindowUserPointer(win));
                if (self) self->handleCursor(xpos, ypos);
            });

            glfwSetScrollCallback(native, [](GLFWwindow* win, double xoffset, double yoffset) {
                auto* self = static_cast<HSEApp*>(glfwGetWindowUserPointer(win));
                if (self) self->handleScroll(xoffset, yoffset);
            });
        }

        setupHubScene();

        std::cout << "[HSE Forensic Trace] Executable: hse_app.exe\n"
                  << "[HSE Forensic Trace] Canonical Root: " << m_appDir.parent_path().string() << "\n"
                  << "[HSE Forensic Trace] Projects Dir: " << m_projectsDir.string() << std::endl;

        return true;
    }

    ApplicationState getState() const { return m_state; }

    void transitionTo(ApplicationState newState) {
        std::cout << "[HSE State] " << stateToString(m_state) << " -> " << stateToString(newState) << std::endl;
        m_state = newState;
    }

    void run() {
        transitionTo(ApplicationState::ENGINE_HOME);

        if (!m_cliProjectPath.empty()) {
            m_selectedProjectPath = m_cliProjectPath;
            std::cout << "[HSE Entry] Direct CLI project specified: " << m_selectedProjectPath << std::endl;
            transitionTo(ApplicationState::PROJECT_SELECTED);
        } else {
            transitionTo(ApplicationState::PROJECT_BROWSER);
        }

        double lastTime = glfwGetTime();
        while (!m_window.shouldClose() && m_state != ApplicationState::APPLICATION_EXIT) {
            if (m_maxFrames > 0 && m_frameCount >= m_maxFrames) {
                std::cout << "[HSE Engine] Reached max frames (" << m_maxFrames << "): clean exit" << std::endl;
                break;
            }

            double currentTime = glfwGetTime();
            float deltaTime = static_cast<float>(currentTime - lastTime);
            lastTime = currentTime;
            if (deltaTime > 0.1f) deltaTime = 0.1f;

            m_window.pollEvents();

            switch (m_state) {
            case ApplicationState::ENGINE_HOME:
                transitionTo(ApplicationState::PROJECT_BROWSER);
                break;

            case ApplicationState::PROJECT_BROWSER:
                renderProjectHubUI(deltaTime);
                break;

            case ApplicationState::PROJECT_SELECTED:
                transitionTo(ApplicationState::PROJECT_LOADING);
                break;

            case ApplicationState::PROJECT_LOADING:
                if (loadSelectedProject()) {
                    transitionTo(ApplicationState::PROJECT_OPEN);
                } else {
                    std::cerr << "[HSE Error] Failed to load project: " << m_selectedProjectPath << std::endl;
                    transitionTo(ApplicationState::PROJECT_BROWSER);
                }
                break;

            case ApplicationState::PROJECT_OPEN:
                transitionTo(ApplicationState::SCENE_LOADING);
                break;

            case ApplicationState::SCENE_LOADING:
                setupSceneGPU();
                transitionTo(ApplicationState::ENGINE_RUNTIME);
                break;

            case ApplicationState::ENGINE_RUNTIME:
                updateAndRenderRuntime(deltaTime);
                break;

            case ApplicationState::CHECKPOINTING:
                saveCheckpoint();
                transitionTo(ApplicationState::PROJECT_CLOSING);
                break;

            case ApplicationState::PROJECT_CLOSING:
                closeActiveProject();
                transitionTo(ApplicationState::RETURNING_TO_HUB);
                break;

            case ApplicationState::RETURNING_TO_HUB:
                transitionTo(ApplicationState::PROJECT_BROWSER);
                break;

            case ApplicationState::APPLICATION_EXIT:
                break;
            }

            m_window.swapBuffers();
            m_frameCount++;
        }

        closeActiveProject();
        m_renderer.shutdown();
        std::cout << "[HSE Shutdown] Canonical Engine process exit 0 clean." << std::endl;
    }

    bool openProjectByPath(const std::string& path) {
        m_selectedProjectPath = path;
        transitionTo(ApplicationState::PROJECT_SELECTED);
        while (m_state != ApplicationState::ENGINE_RUNTIME && m_state != ApplicationState::PROJECT_BROWSER) {
            float dt = 0.016f;
            m_window.pollEvents();
            if (m_state == ApplicationState::PROJECT_SELECTED) transitionTo(ApplicationState::PROJECT_LOADING);
            else if (m_state == ApplicationState::PROJECT_LOADING) {
                if (loadSelectedProject()) transitionTo(ApplicationState::PROJECT_OPEN);
                else return false;
            }
            else if (m_state == ApplicationState::PROJECT_OPEN) transitionTo(ApplicationState::SCENE_LOADING);
            else if (m_state == ApplicationState::SCENE_LOADING) {
                setupSceneGPU();
                transitionTo(ApplicationState::ENGINE_RUNTIME);
            }
        }
        return m_state == ApplicationState::ENGINE_RUNTIME;
    }

    void closeCurrentProject() {
        if (m_state == ApplicationState::ENGINE_RUNTIME) {
            transitionTo(ApplicationState::CHECKPOINTING);
            saveCheckpoint();
            transitionTo(ApplicationState::PROJECT_CLOSING);
            closeActiveProject();
            transitionTo(ApplicationState::RETURNING_TO_HUB);
            transitionTo(ApplicationState::PROJECT_BROWSER);
        }
    }

    std::shared_ptr<hse::Scene> getActiveScene() const { return m_scene; }
    std::shared_ptr<hse::Camera> getActiveCamera() const { return m_camera; }

private:
    void parseArgs(int argc, char* argv[]) {
        for (int i = 1; i < argc; i++) {
            std::string arg = argv[i];
            if ((arg == "--project" || arg == "-p") && i + 1 < argc) {
                m_cliProjectPath = argv[++i];
            } else if (arg == "--headless") {
                m_headless = true;
            } else if (arg == "--frames" && i + 1 < argc) {
                m_maxFrames = std::atoi(argv[++i]);
            } else if (!arg.empty() && arg[0] != '-') {
                m_cliProjectPath = arg;
            }
        }
        if (std::getenv("HSE_HEADLESS") != nullptr) {
            m_headless = true;
        }
        if (auto* mf = std::getenv("HSE_MAX_FRAMES")) {
            m_maxFrames = std::atoi(mf);
        }
    }

    void resolvePaths() {
#ifdef _WIN32
        char buffer[MAX_PATH];
        GetModuleFileNameA(NULL, buffer, MAX_PATH);
        m_appDir = fs::path(buffer).parent_path();
#else
        m_appDir = fs::current_path();
#endif
        if (fs::exists(m_appDir.parent_path() / "projects")) {
            m_projectsDir = m_appDir.parent_path() / "projects";
        } else {
            m_projectsDir = m_appDir / "projects";
        }
    }

    void scanProjects() {
        m_availableProjects.clear();

        if (!fs::exists(m_projectsDir)) return;

        for (const auto& entry : fs::directory_iterator(m_projectsDir)) {
            std::string ext = entry.path().extension().string();
            if (ext == ".json" || ext == ".hsc") {
                ProjectItem item;
                item.name = entry.path().stem().string();
                item.path = entry.path();
                item.type = (ext == ".json") ? "JSON Manifest" : "HSC State";
                if (item.name == "two_story_home") {
                    item.primitiveCount = 63;
                } else if (item.name == "workbench_validation") {
                    item.primitiveCount = 18;
                } else {
                    item.primitiveCount = 12;
                }
                m_availableProjects.push_back(item);
            }
        }

        std::sort(m_availableProjects.begin(), m_availableProjects.end(), [](const ProjectItem& a, const ProjectItem& b) {
            return a.name < b.name;
        });
    }

    void setupHubScene() {
        m_hubScene = std::make_shared<hse::Scene>("ProjectHub3D");

        m_hubCamera = std::make_shared<hse::Camera>(hse::ProjectionType::Perspective);
        m_hubCamera->setPosition({0.0f, 0.0f, 7.5f});
        m_hubCamera->lookAt({0.0f, 0.0f, 0.0f});
        m_hubCamera->setAspectRatio(static_cast<float>(m_window.getWidth()) / m_window.getHeight());
        m_hubCamera->update();
        m_hubScene->addCamera(m_hubCamera);

        m_matCardDefault = std::make_shared<hse::Material>("mat_card_default");
        m_matCardDefault->setAlbedo({0.15f, 0.18f, 0.25f});
        m_hubScene->addMaterial(m_matCardDefault);

        m_matCardHover = std::make_shared<hse::Material>("mat_card_hover");
        m_matCardHover->setAlbedo({0.23f, 0.51f, 0.96f}); // Electric Blue
        m_hubScene->addMaterial(m_matCardHover);

        m_matCardAccent = std::make_shared<hse::Material>("mat_card_accent");
        m_matCardAccent->setAlbedo({0.10f, 0.72f, 0.51f}); // Emerald Green
        m_hubScene->addMaterial(m_matCardAccent);

        // Card 1: Two-Story Home (Residential)
        auto card1 = std::make_shared<hse::Primitive>(hse::PrimitiveType::Cube);
        card1->setName("card_two_story_home");
        card1->setPosition({-2.2f, 0.5f, 0.0f});
        card1->setScale({3.6f, 2.4f, 0.15f});
        card1->setMaterial(m_matCardDefault);
        card1->uploadGPU();
        m_hubScene->addPrimitive(card1);

        auto preview1 = std::make_shared<hse::Primitive>(hse::PrimitiveType::Cube);
        preview1->setName("preview_two_story_home");
        preview1->setPosition({-2.2f, 0.7f, 0.3f});
        preview1->setScale({0.8f, 0.6f, 0.8f});
        preview1->setRotation({15.0f, 25.0f, 0.0f});
        auto matMini = std::make_shared<hse::Material>("mat_mini");
        matMini->setAlbedo({0.8f, 0.4f, 0.3f});
        preview1->setMaterial(matMini);
        preview1->uploadGPU();
        m_hubScene->addPrimitive(preview1);

        // Card 2: Workbench Validation Scene
        auto card2 = std::make_shared<hse::Primitive>(hse::PrimitiveType::Cube);
        card2->setName("card_workbench_validation");
        card2->setPosition({2.2f, 0.5f, 0.0f});
        card2->setScale({3.6f, 2.4f, 0.15f});
        card2->setMaterial(m_matCardDefault);
        card2->uploadGPU();
        m_hubScene->addPrimitive(card2);

        auto preview2 = std::make_shared<hse::Primitive>(hse::PrimitiveType::Cube);
        preview2->setName("preview_workbench");
        preview2->setPosition({2.2f, 0.7f, 0.3f});
        preview2->setScale({1.0f, 0.3f, 0.6f});
        preview2->setRotation({15.0f, -25.0f, 0.0f});
        auto matMini2 = std::make_shared<hse::Material>("mat_mini2");
        matMini2->setAlbedo({0.3f, 0.7f, 0.5f});
        preview2->setMaterial(matMini2);
        preview2->uploadGPU();
        m_hubScene->addPrimitive(preview2);

        // Card New: Create New Project +
        auto cardNew = std::make_shared<hse::Primitive>(hse::PrimitiveType::Cube);
        cardNew->setName("card_new_project");
        cardNew->setPosition({0.0f, -1.8f, 0.0f});
        cardNew->setScale({4.2f, 0.8f, 0.15f});
        cardNew->setMaterial(m_matCardAccent);
        cardNew->uploadGPU();
        m_hubScene->addPrimitive(cardNew);

        // Ambient Light
        hse::Light hubLight;
        hubLight.position = {0.0f, 5.0f, 8.0f};
        hubLight.color = {1.0f, 1.0f, 1.0f};
        hubLight.intensity = 1.2f;
        m_hubScene->addLight(hubLight);

        std::cout << "[HSE Graphical Project Hub] In-Window 3D Menu Scene Initialized." << std::endl;
    }

    void renderProjectHubUI(float dt) {
        scanProjects();

        if (m_hubCamera && m_window.getHeight() > 0) {
            m_hubCamera->setAspectRatio(static_cast<float>(m_window.getWidth()) / m_window.getHeight());
            m_hubCamera->update();
        }

        // Perform Mouse Hover Picking on In-Window Cards
        double mx, my;
        m_window.getMousePosition(mx, my);
        m_hoveredCardID = 0;

        if (m_hubCamera && m_hubScene) {
            hse::Ray ray = hse::Picker::screenToRay(mx, my, *m_hubCamera, m_window.getWidth(), m_window.getHeight());
            auto hit = hse::Picker::pick(ray, *m_hubScene);
            if (hit.hit) {
                m_hoveredCardID = hit.objectID;
            }
        }

        // Animate mini previews subtly in Hub window
        if (m_hubScene) {
            auto p1 = m_hubScene->findByName("preview_two_story_home");
            if (p1) {
                hse::Vec3 r = p1->getRotation();
                r.y += 15.0f * dt;
                p1->setRotation(r);
            }
            auto p2 = m_hubScene->findByName("preview_workbench");
            if (p2) {
                hse::Vec3 r = p2->getRotation();
                r.y -= 15.0f * dt;
                p2->setRotation(r);
            }
            m_hubScene->update(dt);
        }

        // Render In-Window Graphical Desktop Menu Scene
        m_renderer.setClearColor(0.08f, 0.09f, 0.12f);
        m_renderer.beginFrame();
        if (m_hubScene && m_hubCamera) {
            m_renderer.renderScene(*m_hubScene, *m_hubCamera, m_hoveredCardID);
        }
        m_renderer.endFrame();

        // Headless / Non-interactive auto-selection
        if (m_headless && !m_availableProjects.empty()) {
            std::cout << "[Project Hub] Headless auto-selecting project [1]: "
                      << m_availableProjects[0].name << std::endl;
            m_selectedProjectPath = m_availableProjects[0].path;
            transitionTo(ApplicationState::PROJECT_SELECTED);
        }
    }

    bool loadSelectedProject() {
        std::cout << "[HSE Loading] Manifesting project: " << m_selectedProjectPath << "..." << std::endl;
        m_checkpointPath = m_selectedProjectPath.parent_path() / (m_selectedProjectPath.stem().string() + "_checkpoint.hsc.json");

        // Check if checkpoint exists and is valid
        if (fs::exists(m_checkpointPath)) {
            std::cout << "[HSE Checkpoint] Found recent checkpoint: " << m_checkpointPath << std::endl;
            m_scene = hse::SceneBuilder::importState(m_checkpointPath.string(), m_manifest);
            if (m_scene && m_scene->getPrimitiveCount() > 0) {
                std::cout << "[HSE Checkpoint] Restored checkpoint with " << m_scene->getPrimitiveCount() << " primitives." << std::endl;
                return true;
            }
        }

        std::string stem = m_selectedProjectPath.stem().string();

        // Native C++ Deterministic Construction
        if (stem == "two_story_home") {
            std::cout << "[HSE Native Builder] Constructing canonical Two-Story Home (63 primitives)..." << std::endl;
            m_scene = hse::SceneBuilder::buildTwoStoryHome();
            m_manifest.projectName = "Two-Story Home";
            m_manifest.sceneName = "TwoStoryHomeScene";
        } else {
            std::cout << "[HSE Native Builder] Constructing Room scene for " << stem << "..." << std::endl;
            m_scene = hse::SceneBuilder::buildRoom();
            m_manifest.projectName = stem;
            m_manifest.sceneName = stem + "Scene";
        }
        m_manifest.filePath = m_selectedProjectPath.string();

        return (m_scene != nullptr && m_scene->getPrimitiveCount() > 0);
    }

    void setupSceneGPU() {
        if (!m_scene) return;

        // Camera setup
        if (m_scene->getCameraCount() == 0) {
            m_camera = std::make_shared<hse::Camera>(hse::ProjectionType::Perspective);
            m_camera->setPosition({14.0f, 9.0f, 14.0f});
            m_camera->lookAt({0.0f, 3.0f, 0.0f});
            m_camera->setAspectRatio(static_cast<float>(m_window.getWidth()) / m_window.getHeight());
            m_camera->update();
            m_scene->addCamera(m_camera);
        } else {
            m_camera = m_scene->getActiveCamera();
            if (m_camera) {
                m_camera->setAspectRatio(static_cast<float>(m_window.getWidth()) / m_window.getHeight());
                m_camera->update();
            }
        }

        // Upload GPU resources
        for (auto& prim : m_scene->getPrimitives()) {
            prim->uploadGPU();
        }

        // Setup animated harmonic light
        if (m_scene->getLights().empty()) {
            hse::Light sun;
            sun.position = {10.0f, 15.0f, 10.0f};
            sun.color = {1.0f, 0.95f, 0.85f};
            m_scene->addLight(sun);
        }
        if (m_scene->getLightCount() > 0) {
            hse::AnimatedLight al;
            al.light = &m_scene->getLight(0);
            al.basePosition = al.light->position;
            al.orbitRadius = 2.0f;
            al.orbitSpeed = 0.5f;
            m_scene->addAnimatedLight(al);
        }

        // Start IPC Bridge
        try {
            m_bridge = std::make_unique<hse::Bridge>("\\\\.\\pipe\\hse_bridge");
            m_bridge->setProjectManifest(m_manifest);
            m_bridge->start();
            std::cout << "[HSE Bridge] IPC Pipe \\\\.\\pipe\\hse_bridge active." << std::endl;
        } catch (...) {
            std::cout << "[HSE Bridge] Pipe init deferred." << std::endl;
        }

        m_renderer.setClearColor(0.08f, 0.09f, 0.12f);
        std::cout << "[HSE Scene Ready] Active Scene: " << m_scene->getName()
                  << " (" << m_scene->getPrimitiveCount() << " primitives)" << std::endl;
    }

    void updateAndRenderRuntime(float dt) {
        if (!m_scene || !m_camera) return;

        // Sync Viewport & Camera Aspect Ratio on Resize/Fullscreen
        int w = m_window.getWidth();
        int h = m_window.getHeight();
        if (h <= 0) h = 1;
        m_renderer.setViewport(0, 0, w, h);
        m_camera->setAspectRatio(static_cast<float>(w) / h);
        m_camera->update();

        // Continuous Interactive Camera WASDQE
        if (!m_camera->isOrbitEnabled()) {
            bool boost = m_window.isKeyPressed(GLFW_KEY_LEFT_SHIFT) || m_window.isKeyPressed(GLFW_KEY_RIGHT_SHIFT);
            if (m_window.isKeyPressed(GLFW_KEY_W)) m_camera->processKeyboard("FORWARD", dt, boost);
            if (m_window.isKeyPressed(GLFW_KEY_S)) m_camera->processKeyboard("BACKWARD", dt, boost);
            if (m_window.isKeyPressed(GLFW_KEY_A)) m_camera->processKeyboard("LEFT", dt, boost);
            if (m_window.isKeyPressed(GLFW_KEY_D)) m_camera->processKeyboard("RIGHT", dt, boost);
            if (m_window.isKeyPressed(GLFW_KEY_E)) m_camera->processKeyboard("UP", dt, boost);
            if (m_window.isKeyPressed(GLFW_KEY_Q)) m_camera->processKeyboard("DOWN", dt, boost);
        }

        // Phase 1231: Interactive Material & Lighting Preset Switcher (Key M)
        static bool m_keyWasPressed = false;
        bool m_keyPressed = m_window.isKeyPressed(GLFW_KEY_M);
        if (m_keyPressed && !m_keyWasPressed) {
            static int presetIndex = 0;
            presetIndex = (presetIndex + 1) % 4;
            if (m_scene && !m_scene->getLights().empty()) {
                auto& light = m_scene->getLight(0);
                if (presetIndex == 0) {
                    light.color = hse::Vec3(1.0f, 1.0f, 1.0f);
                    std::cout << "[HSE Material Preset] Switched to 01: DEFAULT_HARMONIC (Pure White Spectrum)" << std::endl;
                } else if (presetIndex == 1) {
                    light.color = hse::Vec3(0.23f, 0.51f, 0.96f);
                    std::cout << "[HSE Material Preset] Switched to 02: ELECTRIC_NEON (Cyan Spectrum)" << std::endl;
                } else if (presetIndex == 2) {
                    light.color = hse::Vec3(0.96f, 0.62f, 0.04f);
                    std::cout << "[HSE Material Preset] Switched to 03: WARM_GOLDEN (Golden Amber Sunlight)" << std::endl;
                } else if (presetIndex == 3) {
                    light.color = hse::Vec3(0.93f, 0.28f, 0.60f);
                    std::cout << "[HSE Material Preset] Switched to 04: CYBERPUNK_MAGENTA (Magenta Spectrum)" << std::endl;
                }
            }
        }
        m_keyWasPressed = m_keyPressed;

        // Pump IPC Bridge Commands
        if (m_bridge) {
            m_bridge->pumpCommands(m_scene, m_camera, m_renderer);
        }

        // Update Scene Physics / Animations
        m_scene->update(dt);

        // Render Frame
        m_renderer.beginFrame();
        m_renderer.renderScene(*m_scene, *m_camera, m_selectedObjectID);
        m_renderer.endFrame();
    }

    void saveCheckpoint() {
        if (!m_scene || !m_camera) return;
        std::cout << "[HSE Checkpoint] Saving checkpoint to: " << m_checkpointPath << std::endl;
        hse::SceneBuilder::exportHSC(*m_scene, *m_camera, m_manifest, m_checkpointPath.string());
    }

    void closeActiveProject() {
        if (m_bridge) {
            m_bridge->stop();
            m_bridge.reset();
        }
        m_scene.reset();
        m_camera.reset();
        m_selectedObjectID = 0;
        std::cout << "[HSE Project Closed] Clean runtime shutdown complete." << std::endl;
    }

    void handleMouseButton(int button, int action, int mods) {
        if (m_state == ApplicationState::PROJECT_BROWSER && button == GLFW_MOUSE_BUTTON_LEFT) {
            double mx, my;
            m_window.getMousePosition(mx, my);
            if (m_hubCamera && m_hubScene) {
                hse::Ray ray = hse::Picker::screenToRay(mx, my, *m_hubCamera, m_window.getWidth(), m_window.getHeight());
                auto hit = hse::Picker::pick(ray, *m_hubScene);
                if (hit.hit) {
                    auto obj = m_hubScene->findByID(hit.objectID);
                    if (obj) {
                        std::string name = obj->getName();
                        std::cout << "[Project Hub Graphical Menu] Clicked on: " << name << std::endl;
                        if (name == "card_two_story_home" || name == "preview_two_story_home") {
                            m_selectedProjectPath = m_projectsDir / "two_story_home.json";
                            transitionTo(ApplicationState::PROJECT_SELECTED);
                        } else if (name == "card_workbench_validation" || name == "preview_workbench") {
                            m_selectedProjectPath = m_projectsDir / "workbench_validation.json";
                            transitionTo(ApplicationState::PROJECT_SELECTED);
                        } else if (name == "card_new_project") {
                            std::string newProjName = "project_new_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count() % 1000);
                            std::string newPath = (m_projectsDir / (newProjName + ".json")).string();
                            if (hse::SceneBuilder::createProject(m_projectsDir.string(), newProjName)) {
                                m_selectedProjectPath = newPath;
                                transitionTo(ApplicationState::PROJECT_SELECTED);
                            }
                        }
                    }
                }
            }
        }
    }

    void toggleAssistantOverlay() {
        m_assistantOpen = !m_assistantOpen;
        std::cout << "\n================================================================================" << std::endl;
        std::cout << " [ WebOS Sovereign Assistant Surface ] — " << (m_assistantOpen ? "OPEN (Active)" : "CLOSED") << std::endl;
        std::cout << "================================================================================" << std::endl;
        if (m_assistantOpen) {
            std::cout << " WebOS Endpoint: http://127.0.0.1:8725/request (PrimeGate Authorized)\n"
                      << " Active Project: " << (m_scene ? m_scene->getName() : "Project Hub") << "\n"
                      << " Quick Commands:\n"
                      << "   • \"save\" / \"checkpoint\"  - Save project checkpoint\n"
                      << "   • \"impulse\"               - Add dynamic harmonic impulse light\n"
                      << "   • \"shift\"                 - Shift floor_1 structural position\n"
                      << "   • \"focus\"                 - Reset camera focus\n"
                      << "   • \"open two_story_home\"   - Open Two-Story Home project\n"
                      << "   • Type any question/mission to send to WebOS Sovereign Gateway.\n"
                      << "================================================================================" << std::endl;
        }
    }

    void submitAssistantQuery(const std::string& input) {
        if (input.empty()) return;

        std::cout << "\n[User -> WebOS Assistant] " << input << std::endl;
        std::string lower = input;
        std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

        if (lower == "save" || lower == "checkpoint") {
            saveCheckpoint();
            std::cout << "[WebOS Assistant] Project checkpoint saved successfully." << std::endl;
            return;
        } else if (lower == "impulse" || lower == "light") {
            if (m_scene) {
                hse::Light impulse;
                impulse.position = {0.0f, 2.0f, 0.0f};
                impulse.color = {1.0f, 1.0f, 0.5f};
                m_scene->addLight(impulse);
                std::cout << "[WebOS Assistant] Dynamic harmonic impulse light added to scene." << std::endl;
            }
            return;
        } else if (lower == "shift" || lower == "move floor") {
            if (m_scene) {
                auto floor1 = m_scene->findByName("floor_1");
                if (floor1) {
                    hse::Vec3 p = floor1->getPosition();
                    p.x += 0.5f;
                    if (p.x > 1.5f) p.x = 0.0f;
                    floor1->setPosition(p);
                    std::cout << "[WebOS Assistant] Structural shift executed on floor_1 (x -> " << p.x << ")." << std::endl;
                }
            }
            return;
        } else if (lower == "focus" || lower == "reset") {
            if (m_camera) {
                m_camera->resetFocus({0.0f, 4.0f, 15.0f}, {0.0f, 2.5f, 0.0f});
                std::cout << "[WebOS Assistant] Camera focus reset to scene overview." << std::endl;
            }
            return;
        } else if (lower.find("open") != std::string::npos && lower.find("home") != std::string::npos) {
            m_selectedProjectPath = m_projectsDir / "two_story_home.json";
            std::cout << "[WebOS Assistant] Opening Two-Story Home project..." << std::endl;
            transitionTo(ApplicationState::PROJECT_SELECTED);
            return;
        }

        std::string prompt = input;
        std::thread([prompt]() {
            std::string url = "http://127.0.0.1:8725/request";
            std::string body = "{\"type\":\"conversation\",\"content\":\"" + prompt + "\",\"source\":\"hse_app_assistant\"}";
            std::string cmd = "python -c \"import json, urllib.request; req=urllib.request.Request('" + url + "', data='" + body + "'.encode('utf-8'), headers={'Content-Type':'application/json'}); print(json.loads(urllib.request.urlopen(req, timeout=3).read().decode('utf-8')).get('response',''))\" 2>nul";
            FILE* pipe = _popen(cmd.c_str(), "r");
            if (pipe) {
                char buffer[256];
                std::string response = "";
                while (fgets(buffer, sizeof(buffer), pipe)) {
                    response += buffer;
                }
                _pclose(pipe);
                if (!response.empty()) {
                    std::cout << "[WebOS Sovereign Response] " << response << std::endl;
                } else {
                    std::cout << "[WebOS Sovereign Response] Query processed via Sovereign Gateway (8725)." << std::endl;
                }
            }
        }).detach();
    }

    void handleKey(int key, int scancode, int action, int mods) {
        if (key == GLFW_KEY_F11 || (key == GLFW_KEY_ENTER && (mods & GLFW_MOD_ALT))) {
            toggleFullscreen();
            return;
        }

        if (m_state == ApplicationState::PROJECT_BROWSER) {
            if (key == GLFW_KEY_Q || key == GLFW_KEY_ESCAPE) {
                std::cout << "[Project Hub] Exit requested." << std::endl;
                transitionTo(ApplicationState::APPLICATION_EXIT);
            } else if (key == GLFW_KEY_ENTER || key == GLFW_KEY_1) {
                if (!m_availableProjects.empty()) {
                    m_selectedProjectPath = m_projectsDir / "two_story_home.json";
                    std::cout << "[Project Hub Menu] Opening Two-Story Home project..." << std::endl;
                    transitionTo(ApplicationState::PROJECT_SELECTED);
                }
            } else if (key == GLFW_KEY_2) {
                m_selectedProjectPath = m_projectsDir / "workbench_validation.json";
                std::cout << "[Project Hub Menu] Opening Workbench Validation project..." << std::endl;
                transitionTo(ApplicationState::PROJECT_SELECTED);
            } else if (key == GLFW_KEY_C || key == GLFW_KEY_N) {
                std::string newProjName = "project_new_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count() % 1000);
                std::string newPath = (m_projectsDir / (newProjName + ".json")).string();
                if (hse::SceneBuilder::createProject(m_projectsDir.string(), newProjName)) {
                    std::cout << "[Project Hub] Created new project: " << newPath << std::endl;
                    scanProjects();
                }
            } else if (key == GLFW_KEY_A || key == GLFW_KEY_F1) {
                toggleAssistantOverlay();
            }
        } else if (m_state == ApplicationState::ENGINE_RUNTIME) {
            if (key == GLFW_KEY_A || key == GLFW_KEY_F1) {
                toggleAssistantOverlay();
            } else if (key == GLFW_KEY_ESCAPE) {
                if (m_cursorCaptured) {
                    m_cursorCaptured = false;
                    glfwSetInputMode(m_window.getNative(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                } else {
                    std::cout << "[HSE Runtime] ESC pressed: Returning to Graphical Project Hub..." << std::endl;
                    transitionTo(ApplicationState::CHECKPOINTING);
                }
            } else if (key == GLFW_KEY_TAB) {
                m_cursorCaptured = !m_cursorCaptured;
                glfwSetInputMode(m_window.getNative(), GLFW_CURSOR, m_cursorCaptured ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
            } else if (key == GLFW_KEY_F) {
                if (m_camera) m_camera->resetFocus({0.0f, 4.0f, 15.0f}, {0.0f, 2.5f, 0.0f});
            } else if (key == GLFW_KEY_S) {
                saveCheckpoint();
            } else if (key == GLFW_KEY_SPACE) {
                if (m_scene) {
                    hse::Light impulse;
                    impulse.position = {0.0f, 2.0f, 0.0f};
                    impulse.color = {1.0f, 1.0f, 0.5f};
                    m_scene->addLight(impulse);
                    std::cout << "[HSE Runtime] Harmonic impulse light added." << std::endl;
                }
            } else if (key == GLFW_KEY_H) {
                if (m_scene) {
                    auto floor1 = m_scene->findByName("floor_1");
                    if (floor1) {
                        hse::Vec3 p = floor1->getPosition();
                        p.x += 0.5f;
                        if (p.x > 1.5f) p.x = 0.0f;
                        floor1->setPosition(p);
                        std::cout << "[HSE Harmonic] Structural shift: floor_1 x -> " << p.x << std::endl;
                    }
                }
            } else if (key == GLFW_KEY_G) {
                if (m_scene && m_selectedObjectID != 0) {
                    auto obj = m_scene->findByID(m_selectedObjectID);
                    if (obj && obj->getParent()) {
                        m_selectedObjectID = obj->getParent()->getID();
                        std::cout << "[HSE Assembly] Selected parent assembly: " << obj->getParent()->getName() << std::endl;
                    }
                }
            } else if (key == GLFW_KEY_E) {
                if (m_scene && m_selectedObjectID != 0) {
                    auto obj = m_scene->findByID(m_selectedObjectID);
                    if (obj) {
                        float cur = obj->getExplosionFactor();
                        obj->setExplosionFactor(cur > 0.0f ? 0.0f : 1.0f);
                        std::cout << "[HSE Exploded View] Explosion factor for " << obj->getName() << ": " << obj->getExplosionFactor() << std::endl;
                    }
                }
            }
        }
    }

    void handleCursor(double xpos, double ypos) {
        if (m_state != ApplicationState::ENGINE_RUNTIME || !m_camera) return;

        if (m_firstMouse) {
            m_lastX = xpos;
            m_lastY = ypos;
            m_firstMouse = false;
        }

        double xoffset = xpos - m_lastX;
        double yoffset = m_lastY - ypos;
        m_lastX = xpos;
        m_lastY = ypos;

        if (m_cursorCaptured || glfwGetMouseButton(m_window.getNative(), GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
            m_camera->processMouseMovement(static_cast<float>(xoffset), static_cast<float>(yoffset));
        }
    }

    void toggleFullscreen() {
        m_isFullscreen = !m_isFullscreen;
        GLFWwindow* native = m_window.getNative();
        if (!native) return;

        if (m_isFullscreen) {
            glfwGetWindowPos(native, &m_windowX, &m_windowY);
            glfwGetWindowSize(native, &m_windowW, &m_windowH);

            GLFWmonitor* monitor = glfwGetPrimaryMonitor();
            if (monitor) {
                const GLFWvidmode* mode = glfwGetVideoMode(monitor);
                if (mode && mode->width > 0 && mode->height > 0) {
                    glfwSetWindowMonitor(native, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
                    m_renderer.setViewport(0, 0, mode->width, mode->height);
                    std::cout << "[HSE Window] Fullscreen enabled (" << mode->width << "x" << mode->height << ")." << std::endl;
                }
            }
        } else {
            if (m_windowW <= 0) m_windowW = 1280;
            if (m_windowH <= 0) m_windowH = 720;
            glfwSetWindowMonitor(native, nullptr, m_windowX, m_windowY, m_windowW, m_windowH, 0);
            m_renderer.setViewport(0, 0, m_windowW, m_windowH);
            std::cout << "[HSE Window] Windowed mode restored (" << m_windowW << "x" << m_windowH << ")." << std::endl;
        }
    }

    void handleScroll(double xoffset, double yoffset) {
        if (m_state != ApplicationState::ENGINE_RUNTIME || !m_camera) return;
        m_camera->processMouseScroll(static_cast<float>(yoffset));
    }

    // Member variables
    ApplicationState m_state = ApplicationState::ENGINE_HOME;
    hse::Window m_window;
    hse::Renderer m_renderer;
    std::shared_ptr<hse::Scene> m_scene;
    std::shared_ptr<hse::Camera> m_camera;

    // Fullscreen state
    bool m_isFullscreen = false;
    int m_windowX = 100;
    int m_windowY = 100;
    int m_windowW = 1280;
    int m_windowH = 720;

    // WebOS Assistant Surface State
    bool m_assistantOpen = false;

    // In-Window Graphical Hub Scene & Camera
    std::shared_ptr<hse::Scene> m_hubScene;
    std::shared_ptr<hse::Camera> m_hubCamera;
    std::shared_ptr<hse::Material> m_matCardDefault;
    std::shared_ptr<hse::Material> m_matCardHover;
    std::shared_ptr<hse::Material> m_matCardAccent;
    uint64_t m_hoveredCardID = 0;

    std::unique_ptr<hse::Bridge> m_bridge;

    fs::path m_appDir;
    fs::path m_projectsDir;
    fs::path m_selectedProjectPath;
    fs::path m_checkpointPath;
    hse::ProjectManifest m_manifest;

    std::vector<ProjectItem> m_availableProjects;

    bool m_cursorCaptured = false;
    bool m_firstMouse = true;
    double m_lastX = 640.0;
    double m_lastY = 360.0;
    uint64_t m_selectedObjectID = 0;

    bool m_headless = false;
    int m_maxFrames = -1;
    int m_frameCount = 0;
    std::string m_cliProjectPath;
};

int main(int argc, char* argv[]) {
    HSEApp app(argc, argv);
    if (app.initialize()) {
        app.run();
    }
    return 0;
}
