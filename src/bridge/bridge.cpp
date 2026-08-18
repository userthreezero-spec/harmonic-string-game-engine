#include "bridge/bridge.h"
#include "scene/primitive.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <thread>
#include <cmath>

namespace hse {

Bridge::Bridge(const std::string& pipeName) : m_pipe(pipeName) {}

Bridge::~Bridge() { stop(); }

bool Bridge::start() {
    if (!m_pipe.start()) return false;
    m_running = true;
    m_readerThread = std::thread(&Bridge::readerThread, this);
    while (!m_readerReady.load()) std::this_thread::sleep_for(std::chrono::milliseconds(10));
    std::cout << "Bridge started. Waiting for client..." << std::endl;
    return true;
}

void Bridge::stop() {
    m_running = false;
    if (m_readerThread.joinable()) m_readerThread.join();
    m_pipe.stop();
}

void Bridge::readerThread() {
    m_readerReady = true;
    while (m_running) {
        if (!m_pipe.isClientConnected()) {
            if (!m_pipe.waitClient(1000)) continue;
            std::cout << "Client connected" << std::endl;
        }
        std::string line;
        if (!m_pipe.readLine(line)) continue;

        Command cmd;
        std::string err = parseCommand(line, cmd);
        if (!err.empty()) {
            m_pipe.writeLine(makeAck(0, false, "\"error\":\"" + err + "\""));
            continue;
        }
        if (!m_queue.push(cmd)) {
            m_pipe.writeLine(makeAck(cmd.seq, false, "\"error\":\"queue_full\""));
            continue;
        }
    }
}

std::string Bridge::makeAck(uint64_t seq, bool ok, const std::string& extra) {
    std::ostringstream o;
    o << "{\"type\":\"command_ack\",\"seq\":" << seq;
    o << ",\"status\":\"" << (ok ? "accepted" : "failed") << "\"";
    if (!extra.empty()) o << "," << extra;
    o << "}";
    return o.str();
}

std::string Bridge::parseCommand(const std::string& json, Command& cmd) {
    auto find = [&](const std::string& key) -> std::string {
        auto pos = json.find("\"" + key + "\"");
        if (pos == std::string::npos) return "";
        pos = json.find(':', pos);
        if (pos == std::string::npos) return "";
        pos++;
        while (pos < json.size() && json[pos] == ' ') pos++;
        if (pos >= json.size()) return "";
        if (json[pos] == '"') {
            pos++;
            auto end = json.find('"', pos);
            if (end == std::string::npos) return "";
            return json.substr(pos, end - pos);
        }
        auto end = json.find_first_of(",}", pos);
        if (end == std::string::npos) return json.substr(pos);
        return json.substr(pos, end - pos);
    };

    auto findFloat = [&](const std::string& key, float def) -> float {
        std::string val = find(key);
        return val.empty() ? def : std::stof(val);
    };

    auto findInt = [&](const std::string& key, int def) -> int {
        std::string val = find(key);
        return val.empty() ? def : std::stoi(val);
    };

    auto findU64 = [&](const std::string& key, uint64_t def) -> uint64_t {
        std::string val = find(key);
        return val.empty() ? def : std::stoull(val);
    };

    cmd.seq = findU64("seq", 0);
    std::string cmdType = find("cmd");
    if (cmdType.empty()) cmdType = find("operation");
    if (cmdType.empty()) return "missing cmd";

    if (cmdType == "add_primitive") {
        cmd.type = Command::CMD_ADD_PRIMITIVE;
        std::string primType = find("type");
        if (primType == "Triangle") cmd.primType = 0;
        else if (primType == "Quad") cmd.primType = 1;
        else if (primType == "Cube") cmd.primType = 2;
        else if (primType == "Sphere") cmd.primType = 3;
        else return "invalid type: " + primType;
        cmd.px = findFloat("px", 0); cmd.py = findFloat("py", 0); cmd.pz = findFloat("pz", 0);
        cmd.rx = findFloat("rx", 0); cmd.ry = findFloat("ry", 0); cmd.rz = findFloat("rz", 0);
        cmd.sx = findFloat("sx", 1); cmd.sy = findFloat("sy", 1); cmd.sz = findFloat("sz", 1);
        cmd.cr = findFloat("cr", 1); cmd.cg = findFloat("cg", 1); cmd.cb = findFloat("cb", 1);
        std::string name = find("name");
        if (!name.empty()) strncpy(cmd.name, name.c_str(), sizeof(cmd.name) - 1);
        return "";
    }
    if (cmdType == "remove_primitive") {
        cmd.type = Command::CMD_REMOVE_PRIMITIVE;
        cmd.objectID = findU64("object_id", 0);
        cmd.index = findInt("index", -1);
        return "";
    }
    if (cmdType == "set_position") {
        cmd.type = Command::CMD_SET_POSITION;
        cmd.objectID = findU64("object_id", 0);
        cmd.index = findInt("index", -1);
        cmd.px = findFloat("px", 0); cmd.py = findFloat("py", 0); cmd.pz = findFloat("pz", 0);
        return "";
    }
    if (cmdType == "set_rotation") {
        cmd.type = Command::CMD_SET_ROTATION;
        cmd.objectID = findU64("object_id", 0);
        cmd.index = findInt("index", -1);
        cmd.rx = findFloat("rx", 0); cmd.ry = findFloat("ry", 0); cmd.rz = findFloat("rz", 0);
        return "";
    }
    if (cmdType == "set_scale") {
        cmd.type = Command::CMD_SET_SCALE;
        cmd.objectID = findU64("object_id", 0);
        cmd.index = findInt("index", -1);
        cmd.sx = findFloat("sx", 1); cmd.sy = findFloat("sy", 1); cmd.sz = findFloat("sz", 1);
        return "";
    }
    if (cmdType == "set_color") {
        cmd.type = Command::CMD_SET_COLOR;
        cmd.objectID = findU64("object_id", 0);
        cmd.index = findInt("index", -1);
        cmd.cr = findFloat("cr", 1); cmd.cg = findFloat("cg", 1); cmd.cb = findFloat("cb", 1);
        return "";
    }
    if (cmdType == "set_camera") {
        cmd.type = Command::CMD_SET_CAMERA;
        cmd.px = findFloat("px", 5); cmd.py = findFloat("py", 5); cmd.pz = findFloat("pz", 5);
        cmd.rx = findFloat("rx", 0); cmd.ry = findFloat("ry", 0); cmd.rz = findFloat("rz", 0);
        cmd.fov = findFloat("fov", 60);
        return "";
    }
    if (cmdType == "set_clear_color") {
        cmd.type = Command::CMD_SET_CLEAR_COLOR;
        cmd.cr = findFloat("r", 0.1f); cmd.cg = findFloat("g", 0.1f); cmd.cb = findFloat("b", 0.15f);
        return "";
    }
    if (cmdType == "observe_scene" || cmdType == "scene_state") {
        cmd.type = Command::CMD_OBSERVE_SCENE;
        return "";
    }
    if (cmdType == "observe_object") {
        cmd.type = Command::CMD_OBSERVE_OBJECT;
        cmd.objectID = findU64("object_id", 0);
        return "";
    }
    if (cmdType == "observe_camera") {
        cmd.type = Command::CMD_OBSERVE_CAMERA;
        return "";
    }
    if (cmdType == "observe_health" || cmdType == "health") {
        cmd.type = Command::CMD_OBSERVE_HEALTH;
        return "";
    }
    if (cmdType == "capture_frame") {
        cmd.type = Command::CMD_CAPTURE_FRAME;
        return "";
    }
    if (cmdType == "observe_delta") {
        cmd.type = Command::CMD_OBSERVE_SCENE;
        cmd.sinceRevision = findU64("since_revision", 0);
        return "";
    }
    if (cmdType == "save_project") {
        cmd.type = Command::CMD_SAVE_PROJECT;
        std::string path = find("path");
        if (!path.empty()) strncpy(cmd.name, path.c_str(), sizeof(cmd.name) - 1);
        return "";
    }
    if (cmdType == "load_project") {
        cmd.type = Command::CMD_LOAD_PROJECT;
        std::string path = find("path");
        if (!path.empty()) strncpy(cmd.name, path.c_str(), sizeof(cmd.name) - 1);
        return "";
    }
    return "unknown cmd: " + cmdType;
}

// Helper to resolve object by ID or index
static std::shared_ptr<Primitive> resolveObject(const Command& cmd, std::shared_ptr<Scene> scene) {
    if (cmd.objectID > 0) return scene->findByID(cmd.objectID);
    if (cmd.index >= 0 && cmd.index < static_cast<int>(scene->getPrimitiveCount())) {
        return scene->getPrimitives()[cmd.index];
    }
    return nullptr;
}

void Bridge::executeCommand(const Command& cmd, std::shared_ptr<Scene> scene, std::shared_ptr<Camera> camera, Renderer& renderer) {
    m_lastCommandID = "cmd_" + std::to_string(cmd.seq);

    switch (cmd.type) {
        case Command::CMD_ADD_PRIMITIVE: {
            auto prim = std::make_shared<Primitive>(static_cast<PrimitiveType>(cmd.primType));
            prim->setPosition({cmd.px, cmd.py, cmd.pz});
            prim->setRotation({cmd.rx, cmd.ry, cmd.rz});
            prim->setScale({cmd.sx, cmd.sy, cmd.sz});
            prim->setColor({cmd.cr, cmd.cg, cmd.cb});
            if (cmd.name[0] != '\0') prim->setName(cmd.name);
            prim->uploadGPU();
            scene->addPrimitive(prim);
            m_sceneRevision++;
            m_commandsProcessed++;
            m_lastCommandStatus = "accepted";
            std::string resp = makeAck(cmd.seq, true,
                "\"object_id\":" + std::to_string(prim->getID()) +
                ",\"index\":" + std::to_string(static_cast<int>(scene->getPrimitiveCount()) - 1) +
                ",\"revision\":" + std::to_string(m_sceneRevision));
            m_pipe.writeLine(resp);
            break;
        }
        case Command::CMD_REMOVE_PRIMITIVE: {
            auto obj = resolveObject(cmd, scene);
            if (obj) {
                uint64_t id = obj->getID();
                scene->removePrimitiveByID(id);
                m_sceneRevision++;
                m_commandsProcessed++;
                m_lastCommandStatus = "accepted";
                m_pipe.writeLine(makeAck(cmd.seq, true, "\"revision\":" + std::to_string(m_sceneRevision)));
            } else {
                m_lastCommandStatus = "failed";
                m_pipe.writeLine(makeAck(cmd.seq, false, "\"error\":\"object_not_found\""));
            }
            break;
        }
        case Command::CMD_SET_POSITION: {
            auto obj = resolveObject(cmd, scene);
            if (obj) {
                obj->setPosition({cmd.px, cmd.py, cmd.pz});
                m_sceneRevision++;
                m_commandsProcessed++;
                m_lastCommandStatus = "accepted";
                m_pipe.writeLine(makeAck(cmd.seq, true, "\"revision\":" + std::to_string(m_sceneRevision)));
            } else {
                m_lastCommandStatus = "failed";
                m_pipe.writeLine(makeAck(cmd.seq, false, "\"error\":\"object_not_found\""));
            }
            break;
        }
        case Command::CMD_SET_ROTATION: {
            auto obj = resolveObject(cmd, scene);
            if (obj) {
                obj->setRotation({cmd.rx, cmd.ry, cmd.rz});
                m_sceneRevision++;
                m_commandsProcessed++;
                m_lastCommandStatus = "accepted";
                m_pipe.writeLine(makeAck(cmd.seq, true, "\"revision\":" + std::to_string(m_sceneRevision)));
            } else {
                m_lastCommandStatus = "failed";
                m_pipe.writeLine(makeAck(cmd.seq, false, "\"error\":\"object_not_found\""));
            }
            break;
        }
        case Command::CMD_SET_SCALE: {
            auto obj = resolveObject(cmd, scene);
            if (obj) {
                obj->setScale({cmd.sx, cmd.sy, cmd.sz});
                m_sceneRevision++;
                m_commandsProcessed++;
                m_lastCommandStatus = "accepted";
                m_pipe.writeLine(makeAck(cmd.seq, true, "\"revision\":" + std::to_string(m_sceneRevision)));
            } else {
                m_lastCommandStatus = "failed";
                m_pipe.writeLine(makeAck(cmd.seq, false, "\"error\":\"object_not_found\""));
            }
            break;
        }
        case Command::CMD_SET_COLOR: {
            auto obj = resolveObject(cmd, scene);
            if (obj) {
                obj->setColor({cmd.cr, cmd.cg, cmd.cb});
                m_sceneRevision++;
                m_commandsProcessed++;
                m_lastCommandStatus = "accepted";
                m_pipe.writeLine(makeAck(cmd.seq, true, "\"revision\":" + std::to_string(m_sceneRevision)));
            } else {
                m_lastCommandStatus = "failed";
                m_pipe.writeLine(makeAck(cmd.seq, false, "\"error\":\"object_not_found\""));
            }
            break;
        }
        case Command::CMD_SET_CAMERA: {
            camera->setPosition({cmd.px, cmd.py, cmd.pz});
            camera->lookAt({cmd.rx, cmd.ry, cmd.rz});
            camera->setFOV(cmd.fov);
            camera->update();
            m_commandsProcessed++;
            m_lastCommandStatus = "accepted";
            m_pipe.writeLine(makeAck(cmd.seq, true));
            break;
        }
        case Command::CMD_SET_CLEAR_COLOR: {
            renderer.setClearColor(cmd.cr, cmd.cg, cmd.cb);
            m_commandsProcessed++;
            m_lastCommandStatus = "accepted";
            m_pipe.writeLine(makeAck(cmd.seq, true));
            break;
        }
        case Command::CMD_OBSERVE_SCENE: {
            if (cmd.sinceRevision > 0) {
                m_pipe.writeLine(getDeltaObservation(cmd.sinceRevision));
            } else {
                m_pipe.writeLine(getObservation(*scene, *camera, renderer));
            }
            break;
        }
        case Command::CMD_OBSERVE_OBJECT: {
            m_pipe.writeLine(getObjectObservation(cmd.objectID, *scene));
            break;
        }
        case Command::CMD_OBSERVE_CAMERA: {
            m_pipe.writeLine(getCameraObservation(*camera));
            break;
        }
        case Command::CMD_OBSERVE_HEALTH: {
            m_pipe.writeLine(getHealthObservation(*scene, renderer));
            break;
        }
        case Command::CMD_CAPTURE_FRAME: {
            m_pipe.writeLine(getCaptureFrame(renderer));
            break;
        }
        case Command::CMD_SAVE_PROJECT: {
            std::string path = cmd.name;
            if (path.empty()) path = m_manifest.filePath;
            if (path.empty()) {
                m_lastCommandStatus = "failed";
                m_pipe.writeLine(makeAck(cmd.seq, false, "\"error\":\"no_project_path\""));
            } else {
                m_manifest.worldRevision = m_sceneRevision;
                SceneBuilder::exportHSC(*scene, *camera, m_manifest, path);
                m_manifest.filePath = path;
                m_lastCommandStatus = "accepted";
                m_pipe.writeLine(makeAck(cmd.seq, true, "\"path\":\"" + path + "\",\"revision\":" + std::to_string(m_sceneRevision)));
            }
            break;
        }
        case Command::CMD_LOAD_PROJECT: {
            std::string path = cmd.name;
            if (path.empty()) {
                m_lastCommandStatus = "failed";
                m_pipe.writeLine(makeAck(cmd.seq, false, "\"error\":\"no_project_path\""));
            } else {
                ProjectManifest newManifest;
                auto newScene = SceneBuilder::importState(path, newManifest);
                if (!newScene) {
                    m_lastCommandStatus = "failed";
                    m_pipe.writeLine(makeAck(cmd.seq, false, "\"error\":\"load_failed\""));
                } else {
                    // Upload GPU resources for all new primitives
                    for (auto& prim : newScene->getPrimitives()) {
                        prim->uploadGPU();
                    }
                    // Note: scene pointer replacement requires caller to handle
                    // For now, we just report success and let the workspace handle it
                    m_manifest = newManifest;
                    m_lastCommandStatus = "accepted";
                    m_pipe.writeLine(makeAck(cmd.seq, true, "\"path\":\"" + path + "\",\"objects\":" + std::to_string(newScene->getPrimitiveCount())));
                }
            }
            break;
        }
        default:
            m_lastCommandStatus = "failed";
            m_pipe.writeLine(makeAck(cmd.seq, false, "\"error\":\"unsupported_operation\""));
            break;
    }
}

void Bridge::pumpCommands(std::shared_ptr<Scene> scene, std::shared_ptr<Camera> camera, Renderer& renderer) {
    Command cmd;
    int pumped = 0;
    while (m_queue.pop(cmd) && pumped < 32) {
        executeCommand(cmd, scene, camera, renderer);
        pumped++;
    }
    m_frameCount = renderer.getFrameCount();
}

static std::string typeToString(PrimitiveType t) {
    switch (t) {
        case PrimitiveType::Triangle: return "triangle";
        case PrimitiveType::Quad: return "quad";
        case PrimitiveType::Cube: return "cube";
        case PrimitiveType::Sphere: return "sphere";
    }
    return "unknown";
}

void Bridge::recordSnapshot(const Scene& scene, const Camera& camera) {
    SceneSnapshot snap;
    snap.revision = m_sceneRevision;
    snap.timestamp = glfwGetTime();
    for (auto& p : scene.getPrimitives()) {
        SceneSnapshot::ObjSnapshot os;
        os.id = p->getID();
        os.name = p->getName();
        os.type = static_cast<int>(p->getType());
        os.px = p->getPosition().x; os.py = p->getPosition().y; os.pz = p->getPosition().z;
        os.rx = p->getRotation().x; os.ry = p->getRotation().y; os.rz = p->getRotation().z;
        os.sx = p->getScale().x; os.sy = p->getScale().y; os.sz = p->getScale().z;
        os.cr = p->getColor().x; os.cg = p->getColor().y; os.cb = p->getColor().z;
        snap.objects.push_back(os);
    }
    snap.camPx = camera.getPosition().x; snap.camPy = camera.getPosition().y; snap.camPz = camera.getPosition().z;
    snap.camTx = camera.getTarget().x; snap.camTy = camera.getTarget().y; snap.camTz = camera.getTarget().z;
    snap.camFov = camera.getFOV();

    m_snapshots.push_back(snap);
    if (m_snapshots.size() > MAX_SNAPSHOTS) m_snapshots.pop_front();
}

std::vector<PropertyChange> Bridge::diffSnapshots(const SceneSnapshot& old, const SceneSnapshot& cur) const {
    std::vector<PropertyChange> changes;
    auto fmtVec3 = [](float x, float y, float z) -> std::string {
        return "[" + std::to_string(x) + "," + std::to_string(y) + "," + std::to_string(z) + "]";
    };

    std::unordered_map<uint64_t, const SceneSnapshot::ObjSnapshot*> oldMap;
    for (auto& o : old.objects) oldMap[o.id] = &o;

    for (auto& curObj : cur.objects) {
        auto it = oldMap.find(curObj.id);
        if (it == oldMap.end()) {
            changes.push_back({std::to_string(curObj.id), "created", "", "true"});
            continue;
        }
        auto& oldObj = *it->second;
        if (oldObj.px != curObj.px || oldObj.py != curObj.py || oldObj.pz != curObj.pz)
            changes.push_back({std::to_string(curObj.id), "position", fmtVec3(oldObj.px,oldObj.py,oldObj.pz), fmtVec3(curObj.px,curObj.py,curObj.pz)});
        if (oldObj.rx != curObj.rx || oldObj.ry != curObj.ry || oldObj.rz != curObj.rz)
            changes.push_back({std::to_string(curObj.id), "rotation", fmtVec3(oldObj.rx,oldObj.ry,oldObj.rz), fmtVec3(curObj.rx,curObj.ry,curObj.rz)});
        if (oldObj.sx != curObj.sx || oldObj.sy != curObj.sy || oldObj.sz != curObj.sz)
            changes.push_back({std::to_string(curObj.id), "scale", fmtVec3(oldObj.sx,oldObj.sy,oldObj.sz), fmtVec3(curObj.sx,curObj.sy,curObj.sz)});
        if (oldObj.cr != curObj.cr || oldObj.cg != curObj.cg || oldObj.cb != curObj.cb)
            changes.push_back({std::to_string(curObj.id), "color", fmtVec3(oldObj.cr,oldObj.cg,oldObj.cb), fmtVec3(curObj.cr,curObj.cg,curObj.cb)});
    }

    for (auto& oldObj : old.objects) {
        bool found = false;
        for (auto& curObj : cur.objects) { if (curObj.id == oldObj.id) { found = true; break; } }
        if (!found) changes.push_back({std::to_string(oldObj.id), "removed", "true", ""});
    }

    return changes;
}

std::string Bridge::getObservation(const Scene& scene, const Camera& camera, const Renderer& renderer) {
    recordSnapshot(scene, camera);
    std::ostringstream o;
    o << std::fixed << std::setprecision(3);
    o << "{\"type\":\"scene_observation\"";
    o << ",\"observation_id\":\"obs_" << m_commandsProcessed << "\"";
    o << ",\"scene_revision\":" << m_sceneRevision;
    o << ",\"timestamp\":" << glfwGetTime();
    o << ",\"frame_count\":" << renderer.getFrameCount();
    o << ",\"commands_processed\":" << m_commandsProcessed;
    o << ",\"pending_commands\":" << (m_queue.empty() ? 0 : 1);
    o << ",\"camera\":{";
    o << "\"position\":[" << camera.getPosition().x << "," << camera.getPosition().y << "," << camera.getPosition().z << "]";
    o << ",\"target\":[" << camera.getTarget().x << "," << camera.getTarget().y << "," << camera.getTarget().z << "]";
    o << ",\"fov\":" << camera.getFOV();
    o << ",\"near\":" << camera.getNearPlane();
    o << ",\"far\":" << camera.getFarPlane();
    o << ",\"aspect\":" << camera.getAspectRatio();
    o << "}";
    o << ",\"objects\":[";
    bool first = true;
    for (auto& p : scene.getPrimitives()) {
        if (!first) o << ",";
        first = false;
        o << "{\"id\":" << p->getID();
        o << ",\"name\":\"" << p->getName() << "\"";
        o << ",\"type\":\"" << typeToString(p->getType()) << "\"";
        o << ",\"position\":[" << p->getPosition().x << "," << p->getPosition().y << "," << p->getPosition().z << "]";
        o << ",\"rotation\":[" << p->getRotation().x << "," << p->getRotation().y << "," << p->getRotation().z << "]";
        o << ",\"scale\":[" << p->getScale().x << "," << p->getScale().y << "," << p->getScale().z << "]";
        o << ",\"color\":[" << p->getColor().x << "," << p->getColor().y << "," << p->getColor().z << "]";
        o << "}";
    }
    o << "]}";
    return o.str();
}

std::string Bridge::getObjectObservation(uint64_t objectID, const Scene& scene) {
    auto obj = scene.findByID(objectID);
    if (!obj) {
        return "{\"type\":\"object_observation\",\"status\":\"failed\",\"error\":\"object_not_found\",\"object_id\":" + std::to_string(objectID) + "}";
    }
    std::ostringstream o;
    o << std::fixed << std::setprecision(3);
    o << "{\"type\":\"object_observation\"";
    o << ",\"id\":" << obj->getID();
    o << ",\"name\":\"" << obj->getName() << "\"";
    o << ",\"type\":\"" << typeToString(obj->getType()) << "\"";
    o << ",\"position\":[" << obj->getPosition().x << "," << obj->getPosition().y << "," << obj->getPosition().z << "]";
    o << ",\"rotation\":[" << obj->getRotation().x << "," << obj->getRotation().y << "," << obj->getRotation().z << "]";
    o << ",\"scale\":[" << obj->getScale().x << "," << obj->getScale().y << "," << obj->getScale().z << "]";
    o << ",\"color\":[" << obj->getColor().x << "," << obj->getColor().y << "," << obj->getColor().z << "]";
    o << "}";
    return o.str();
}

std::string Bridge::getCameraObservation(const Camera& camera) {
    std::ostringstream o;
    o << std::fixed << std::setprecision(3);
    o << "{\"type\":\"camera_observation\"";
    o << ",\"position\":[" << camera.getPosition().x << "," << camera.getPosition().y << "," << camera.getPosition().z << "]";
    o << ",\"target\":[" << camera.getTarget().x << "," << camera.getTarget().y << "," << camera.getTarget().z << "]";
    o << ",\"fov\":" << camera.getFOV();
    o << ",\"near\":" << camera.getNearPlane();
    o << ",\"far\":" << camera.getFarPlane();
    o << ",\"aspect\":" << camera.getAspectRatio();
    o << ",\"projection\":\"" << (camera.getProjectionType() == ProjectionType::Perspective ? "perspective" : "orthographic") << "\"";
    o << "}";
    return o.str();
}

std::string Bridge::getHealthObservation(const Scene& scene, const Renderer& renderer) {
    float cr, cg, cb;
    renderer.getClearColor(cr, cg, cb);
    std::ostringstream o;
    o << "{\"type\":\"health_observation\"";
    o << ",\"renderer_running\":" << (renderer.getFrameCount() > 0 ? "true" : "false");
    o << ",\"bridge_connected\":" << (m_pipe.isClientConnected() ? "true" : "false");
    o << ",\"scene_objects\":" << scene.getPrimitiveCount();
    o << ",\"camera_count\":" << scene.getCameraCount();
    o << ",\"frame_count\":" << renderer.getFrameCount();
    o << ",\"scene_revision\":" << m_sceneRevision;
    o << ",\"commands_processed\":" << m_commandsProcessed;
    o << ",\"last_command\":\"" << m_lastCommandID << "\"";
    o << ",\"last_command_status\":\"" << m_lastCommandStatus << "\"";
    o << ",\"clear_color\":[" << cr << "," << cg << "," << cb << "]";
    o << "}";
    return o.str();
}

std::string Bridge::getDeltaObservation(uint64_t sinceRevision) {
    if (m_snapshots.empty()) {
        return "{\"type\":\"scene_delta\",\"status\":\"failed\",\"error\":\"no_snapshots\",\"since_revision\":" + std::to_string(sinceRevision) + "}";
    }

    const SceneSnapshot* oldSnap = nullptr;
    for (auto it = m_snapshots.rbegin(); it != m_snapshots.rend(); ++it) {
        if (it->revision <= sinceRevision) { oldSnap = &(*it); break; }
    }
    if (!oldSnap) oldSnap = &m_snapshots.front();

    const SceneSnapshot& curSnap = m_snapshots.back();
    auto changes = diffSnapshots(*oldSnap, curSnap);

    std::ostringstream o;
    o << "{\"type\":\"scene_delta\"";
    o << ",\"from_revision\":" << oldSnap->revision;
    o << ",\"to_revision\":" << curSnap.revision;
    o << ",\"change_count\":" << changes.size();
    o << ",\"changes\":[";
    bool first = true;
    for (auto& c : changes) {
        if (!first) o << ",";
        first = false;
        o << "{\"object_id\":\"" << c.objectID << "\"";
        o << ",\"property\":\"" << c.property << "\"";
        o << ",\"old\":" << (c.oldValue.empty() ? "null" : "\"" + c.oldValue + "\"");
        o << ",\"new\":" << (c.newValue.empty() ? "null" : "\"" + c.newValue + "\"");
        o << "}";
    }
    o << "]}";
    return o.str();
}

std::string Bridge::getCaptureFrame(Renderer& renderer) {
    std::vector<uint8_t> rgba;
    int w, h;
    if (!renderer.readPixels(rgba, w, h)) {
        return "{\"type\":\"frame_capture\",\"status\":\"failed\",\"error\":\"readPixels_failed\"}";
    }

    // Convert to base64
    static const char* b64 = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string b64str;
    b64str.reserve(((rgba.size() + 2) / 3) * 4);
    for (size_t i = 0; i < rgba.size(); i += 3) {
        unsigned int n = (static_cast<unsigned int>(rgba[i]) << 16);
        if (i + 1 < rgba.size()) n |= (static_cast<unsigned int>(rgba[i + 1]) << 8);
        if (i + 2 < rgba.size()) n |= static_cast<unsigned int>(rgba[i + 2]);
        b64str += b64[(n >> 18) & 0x3F];
        b64str += b64[(n >> 12) & 0x3F];
        b64str += (i + 1 < rgba.size()) ? b64[(n >> 6) & 0x3F] : '=';
        b64str += (i + 2 < rgba.size()) ? b64[n & 0x3F] : '=';
    }

    // Flip vertically (OpenGL reads bottom-up)
    int rowBytes = w * 4;
    for (int y = 0; y < h / 2; y++) {
        for (int x = 0; x < rowBytes; x++) {
            std::swap(rgba[y * rowBytes + x], rgba[(h - 1 - y) * rowBytes + x]);
        }
    }

    std::ostringstream o;
    o << "{\"type\":\"frame_capture\"";
    o << ",\"width\":" << w;
    o << ",\"height\":" << h;
    o << ",\"format\":\"rgba_base64\"";
    o << ",\"data\":\"" << b64str << "\"";
    o << "}";
    return o.str();
}

} // namespace hse
