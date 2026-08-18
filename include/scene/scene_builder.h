#pragma once
#include "scene/scene.h"
#include <string>
#include <memory>

namespace hse {

struct WorldObject {
    std::string id;
    std::string type;
    float px, py, pz;
    float rx, ry, rz;
    float sx, sy, sz;
    float cr, cg, cb;
};

struct ProjectManifest {
    std::string hscVersion = "1.0";
    std::string engineVersion = "0.1.0";
    std::string projectName;
    std::string description;
    std::string sceneName;
    float clearColorR = 0.05f;
    float clearColorG = 0.05f;
    float clearColorB = 0.08f;
    uint64_t worldRevision = 0;
    std::string filePath;
};

class SceneBuilder {
public:
    static std::shared_ptr<Scene> buildRoom();
    static std::shared_ptr<Scene> importState(const std::string& path, ProjectManifest& manifest);
    static void exportState(const Scene& scene, const std::string& path);
    static void exportHSC(const Scene& scene, const Camera& camera, const ProjectManifest& manifest, const std::string& path);
    static bool createProject(const std::string& dir, const std::string& name);
};

} // namespace hse
