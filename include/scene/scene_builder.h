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

class SceneBuilder {
public:
    static std::shared_ptr<Scene> buildRoom();
    static void exportState(const Scene& scene, const std::string& path);
};

} // namespace hse
