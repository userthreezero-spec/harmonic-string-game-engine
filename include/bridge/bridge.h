#pragma once
#include "bridge/command_queue.h"
#include "bridge/pipe_transport.h"
#include "scene/scene.h"
#include "scene/camera.h"
#include "renderer/renderer.h"
#include <memory>
#include <thread>
#include <atomic>
#include <string>
#include <functional>

namespace hse {

class Bridge {
public:
    Bridge(const std::string& pipeName = "\\\\.\\pipe\\hse_bridge");
    ~Bridge();

    bool start();
    void stop();

    void pumpCommands(std::shared_ptr<Scene> scene, std::shared_ptr<Camera> camera, Renderer& renderer);
    std::string getSceneState(const Scene& scene, const Camera& camera);

    bool isRunning() const { return m_running.load(); }
    int getCommandsProcessed() const { return m_commandsProcessed; }
    int getSceneRevision() const { return m_sceneRevision; }

private:
    void readerThread();

    std::string parseCommand(const std::string& json, Command& cmd);
    void executeCommand(const Command& cmd, std::shared_ptr<Scene> scene, std::shared_ptr<Camera> camera, Renderer& renderer);

    PipeServer m_pipe;
    CommandQueue m_queue;
    std::thread m_readerThread;
    std::atomic<bool> m_running{false};
    std::atomic<bool> m_readerReady{false};
    int m_commandsProcessed = 0;
    int m_sceneRevision = 0;
};

} // namespace hse
