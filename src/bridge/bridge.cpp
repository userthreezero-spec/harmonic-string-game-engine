#include "bridge/bridge.h"
#include "scene/primitive.h"
#include <iostream>
#include <sstream>
#include <thread>

namespace hse {

Bridge::Bridge(const std::string& pipeName) : m_pipe(pipeName) {}

Bridge::~Bridge() {
    stop();
}

bool Bridge::start() {
    if (!m_pipe.start()) return false;

    m_running = true;
    m_readerThread = std::thread(&Bridge::readerThread, this);

    // Wait for reader thread to be ready
    while (!m_readerReady.load()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    std::cout << "Bridge started. Waiting for client..." << std::endl;
    return true;
}

void Bridge::stop() {
    m_running = false;
    if (m_readerThread.joinable()) {
        m_readerThread.join();
    }
    m_pipe.stop();
}

void Bridge::readerThread() {
    m_readerReady = true;

    while (m_running) {
        if (!m_pipe.isClientConnected()) {
            if (!m_pipe.waitClient(1000)) continue;
        }

        std::string line;
        if (!m_pipe.readLine(line)) continue;

        Command cmd;
        std::string err = parseCommand(line, cmd);
        if (!err.empty()) {
            std::string resp = "{\"ok\":false,\"error\":\"" + err + "\"}";
            m_pipe.writeLine(resp);
            continue;
        }

        if (!m_queue.push(cmd)) {
            m_pipe.writeLine("{\"ok\":false,\"error\":\"queue_full\"}");
            continue;
        }

        // For non-query commands, wait for acknowledgment
        if (cmd.type != Command::CMD_SCENE_STATE && cmd.type != Command::CMD_HEALTH) {
            m_pipe.writeLine("{\"ok\":true,\"queued\":true}");
        }
    }
}

std::string Bridge::parseCommand(const std::string& json, Command& cmd) {
    // Minimal JSON parser - finds key-value pairs by string matching
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

    std::string cmdType = find("cmd");
    if (cmdType.empty()) return "missing cmd";

    if (cmdType == "add_primitive") {
        cmd.type = Command::CMD_ADD_PRIMITIVE;
        std::string primType = find("type");
        if (primType == "Triangle") cmd.primType = 0;
        else if (primType == "Quad") cmd.primType = 1;
        else if (primType == "Cube") cmd.primType = 2;
        else if (primType == "Sphere") cmd.primType = 3;
        else return "invalid primitive type: " + primType;

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
        cmd.index = findInt("index", -1);
        if (cmd.index < 0) return "invalid index";
        return "";
    }

    if (cmdType == "set_position") {
        cmd.type = Command::CMD_SET_POSITION;
        cmd.index = findInt("index", -1);
        cmd.px = findFloat("px", 0); cmd.py = findFloat("py", 0); cmd.pz = findFloat("pz", 0);
        if (cmd.index < 0) return "invalid index";
        return "";
    }

    if (cmdType == "set_rotation") {
        cmd.type = Command::CMD_SET_ROTATION;
        cmd.index = findInt("index", -1);
        cmd.rx = findFloat("rx", 0); cmd.ry = findFloat("ry", 0); cmd.rz = findFloat("rz", 0);
        if (cmd.index < 0) return "invalid index";
        return "";
    }

    if (cmdType == "set_scale") {
        cmd.type = Command::CMD_SET_SCALE;
        cmd.index = findInt("index", -1);
        cmd.sx = findFloat("sx", 1); cmd.sy = findFloat("sy", 1); cmd.sz = findFloat("sz", 1);
        if (cmd.index < 0) return "invalid index";
        return "";
    }

    if (cmdType == "set_color") {
        cmd.type = Command::CMD_SET_COLOR;
        cmd.index = findInt("index", -1);
        cmd.cr = findFloat("cr", 1); cmd.cg = findFloat("cg", 1); cmd.cb = findFloat("cb", 1);
        if (cmd.index < 0) return "invalid index";
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

    if (cmdType == "scene_state") {
        cmd.type = Command::CMD_SCENE_STATE;
        return "";
    }

    if (cmdType == "health") {
        cmd.type = Command::CMD_HEALTH;
        return "";
    }

    return "unknown command: " + cmdType;
}

void Bridge::executeCommand(const Command& cmd, std::shared_ptr<Scene> scene, std::shared_ptr<Camera> camera, Renderer& renderer) {
    switch (cmd.type) {
        case Command::CMD_ADD_PRIMITIVE: {
            auto prim = std::make_shared<Primitive>(static_cast<PrimitiveType>(cmd.primType));
            prim->setPosition({cmd.px, cmd.py, cmd.pz});
            prim->setRotation({cmd.rx, cmd.ry, cmd.rz});
            prim->setScale({cmd.sx, cmd.sy, cmd.sz});
            prim->setColor({cmd.cr, cmd.cg, cmd.cb});
            prim->uploadGPU();
            scene->addPrimitive(prim);
            int idx = static_cast<int>(scene->getPrimitiveCount()) - 1;
            std::string resp = "{\"ok\":true,\"index\":" + std::to_string(idx) + ",\"revision\":" + std::to_string(m_sceneRevision) + "}";
            m_pipe.writeLine(resp);
            m_commandsProcessed++;
            m_sceneRevision++;
            break;
        }
        case Command::CMD_REMOVE_PRIMITIVE: {
            if (cmd.index >= 0 && cmd.index < static_cast<int>(scene->getPrimitiveCount())) {
                scene->removePrimitive(cmd.index);
                m_pipe.writeLine("{\"ok\":true,\"revision\":" + std::to_string(m_sceneRevision) + "}");
                m_commandsProcessed++;
                m_sceneRevision++;
            } else {
                m_pipe.writeLine("{\"ok\":false,\"error\":\"invalid_index\"}");
            }
            break;
        }
        case Command::CMD_SET_POSITION: {
            auto& prims = scene->getPrimitives();
            if (cmd.index >= 0 && cmd.index < static_cast<int>(prims.size())) {
                prims[cmd.index]->setPosition({cmd.px, cmd.py, cmd.pz});
                m_pipe.writeLine("{\"ok\":true,\"revision\":" + std::to_string(m_sceneRevision) + "}");
                m_commandsProcessed++;
                m_sceneRevision++;
            } else {
                m_pipe.writeLine("{\"ok\":false,\"error\":\"invalid_index\"}");
            }
            break;
        }
        case Command::CMD_SET_ROTATION: {
            auto& prims = scene->getPrimitives();
            if (cmd.index >= 0 && cmd.index < static_cast<int>(prims.size())) {
                prims[cmd.index]->setRotation({cmd.rx, cmd.ry, cmd.rz});
                m_pipe.writeLine("{\"ok\":true,\"revision\":" + std::to_string(m_sceneRevision) + "}");
                m_commandsProcessed++;
                m_sceneRevision++;
            } else {
                m_pipe.writeLine("{\"ok\":false,\"error\":\"invalid_index\"}");
            }
            break;
        }
        case Command::CMD_SET_SCALE: {
            auto& prims = scene->getPrimitives();
            if (cmd.index >= 0 && cmd.index < static_cast<int>(prims.size())) {
                prims[cmd.index]->setScale({cmd.sx, cmd.sy, cmd.sz});
                m_pipe.writeLine("{\"ok\":true,\"revision\":" + std::to_string(m_sceneRevision) + "}");
                m_commandsProcessed++;
                m_sceneRevision++;
            } else {
                m_pipe.writeLine("{\"ok\":false,\"error\":\"invalid_index\"}");
            }
            break;
        }
        case Command::CMD_SET_COLOR: {
            auto& prims = scene->getPrimitives();
            if (cmd.index >= 0 && cmd.index < static_cast<int>(prims.size())) {
                prims[cmd.index]->setColor({cmd.cr, cmd.cg, cmd.cb});
                m_pipe.writeLine("{\"ok\":true,\"revision\":" + std::to_string(m_sceneRevision) + "}");
                m_commandsProcessed++;
                m_sceneRevision++;
            } else {
                m_pipe.writeLine("{\"ok\":false,\"error\":\"invalid_index\"}");
            }
            break;
        }
        case Command::CMD_SET_CAMERA: {
            camera->setPosition({cmd.px, cmd.py, cmd.pz});
            camera->lookAt({cmd.rx, cmd.ry, cmd.rz});
            camera->setFOV(cmd.fov);
            camera->update();
            m_pipe.writeLine("{\"ok\":true,\"revision\":" + std::to_string(m_sceneRevision) + "}");
            m_commandsProcessed++;
            break;
        }
        case Command::CMD_SET_CLEAR_COLOR: {
            renderer.setClearColor(cmd.cr, cmd.cg, cmd.cb);
            m_pipe.writeLine("{\"ok\":true,\"revision\":" + std::to_string(m_sceneRevision) + "}");
            m_commandsProcessed++;
            break;
        }
        case Command::CMD_SCENE_STATE: {
            std::string state = getSceneState(*scene, *camera);
            m_pipe.writeLine(state);
            break;
        }
        case Command::CMD_HEALTH: {
            std::string resp = "{\"ok\":true,\"status\":\"alive\",\"commands_processed\":" + std::to_string(m_commandsProcessed) + ",\"scene_revision\":" + std::to_string(m_sceneRevision) + ",\"primitive_count\":" + std::to_string(scene->getPrimitiveCount()) + "}";
            m_pipe.writeLine(resp);
            break;
        }
        default:
            m_pipe.writeLine("{\"ok\":false,\"error\":\"unknown_command\"}");
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
}

std::string Bridge::getSceneState(const Scene& scene, const Camera& camera) {
    std::ostringstream o;
    o << "{";
    o << "\"ok\":true";
    o << ",\"primitive_count\":" << scene.getPrimitiveCount();
    o << ",\"camera_count\":" << scene.getCameraCount();
    o << ",\"scene_revision\":" << m_sceneRevision;
    o << ",\"commands_processed\":" << m_commandsProcessed;
    o << ",\"objects\":[";

    auto& prims = scene.getPrimitives();
    for (size_t i = 0; i < prims.size(); i++) {
        auto& p = prims[i];
        if (i > 0) o << ",";
        o << "{\"index\":" << i;
        o << ",\"type\":" << static_cast<int>(p->getType());
        o << ",\"position\":[" << p->getPosition().x << "," << p->getPosition().y << "," << p->getPosition().z << "]";
        o << ",\"rotation\":[" << p->getRotation().x << "," << p->getRotation().y << "," << p->getRotation().z << "]";
        o << ",\"scale\":[" << p->getScale().x << "," << p->getScale().y << "," << p->getScale().z << "]";
        o << ",\"color\":[" << p->getColor().x << "," << p->getColor().y << "," << p->getColor().z << "]";
        o << "}";
    }

    o << "],\"camera\":{";
    o << "\"position\":[" << camera.getPosition().x << "," << camera.getPosition().y << "," << camera.getPosition().z << "]";
    o << ",\"fov\":60";
    o << "}}";

    return o.str();
}

} // namespace hse
