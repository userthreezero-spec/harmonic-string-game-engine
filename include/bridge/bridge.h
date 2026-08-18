#pragma once
#include "bridge/command_queue.h"
#include "bridge/pipe_transport.h"
#include "scene/scene.h"
#include "scene/camera.h"
#include "scene/scene_builder.h"
#include "renderer/renderer.h"
#include <memory>
#include <thread>
#include <atomic>
#include <string>
#include <vector>
#include <deque>

namespace hse {

struct PropertyChange {
    std::string objectID;
    std::string property;
    std::string oldValue;
    std::string newValue;
};

struct SceneSnapshot {
    uint64_t revision;
    double timestamp;
    struct ObjSnapshot {
        uint64_t id;
        std::string name;
        int type;
        float px, py, pz;
        float rx, ry, rz;
        float sx, sy, sz;
        float cr, cg, cb;
    };
    std::vector<ObjSnapshot> objects;
    float camPx, camPy, camPz;
    float camTx, camTy, camTz;
    float camFov;
};

class Bridge {
public:
    Bridge(const std::string& pipeName = "\\\\.\\pipe\\hse_bridge");
    ~Bridge();

    bool start();
    void stop();

    void pumpCommands(std::shared_ptr<Scene> scene, std::shared_ptr<Camera> camera, Renderer& renderer);

    std::string getObservation(const Scene& scene, const Camera& camera, const Renderer& renderer);
    std::string getObjectObservation(uint64_t objectID, const Scene& scene);
    std::string getCameraObservation(const Camera& camera);
    std::string getHealthObservation(const Scene& scene, const Renderer& renderer);
    std::string getDeltaObservation(uint64_t sinceRevision);
    std::string getCaptureFrame(Renderer& renderer);

    void setProjectManifest(const ProjectManifest& manifest) { m_manifest = manifest; }
    const ProjectManifest& getProjectManifest() const { return m_manifest; }

    bool isRunning() const { return m_running.load(); }
    int getCommandsProcessed() const { return m_commandsProcessed; }
    int getSceneRevision() const { return m_sceneRevision; }
    uint64_t getFrameCount() const { return m_frameCount; }

private:
    void readerThread();
    std::string parseCommand(const std::string& json, Command& cmd);
    void executeCommand(const Command& cmd, std::shared_ptr<Scene> scene, std::shared_ptr<Camera> camera, Renderer& renderer);
    std::string makeAck(uint64_t seq, bool ok, const std::string& extra = "");
    void recordSnapshot(const Scene& scene, const Camera& camera);
    std::vector<PropertyChange> diffSnapshots(const SceneSnapshot& old, const SceneSnapshot& cur) const;

    PipeServer m_pipe;
    CommandQueue m_queue;
    std::thread m_readerThread;
    std::atomic<bool> m_running{false};
    std::atomic<bool> m_readerReady{false};
    int m_commandsProcessed = 0;
    int m_sceneRevision = 0;
    uint64_t m_frameCount = 0;
    uint64_t m_seqCounter = 0;

    std::deque<SceneSnapshot> m_snapshots;
    static constexpr size_t MAX_SNAPSHOTS = 128;

    std::string m_lastCommandID;
    std::string m_lastCommandStatus;
    ProjectManifest m_manifest;
};

} // namespace hse
