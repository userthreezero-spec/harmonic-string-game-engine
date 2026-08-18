#include "scene/scene_builder.h"
#include "scene/primitive.h"
#include "scene/camera.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#ifdef _WIN32
#include <windows.h>
#endif

namespace hse {

static std::shared_ptr<Primitive> makeQuad(
    const std::string& id,
    float px, float py, float pz,
    float rx, float ry, float rz,
    float sx, float sy, float sz,
    float cr, float cg, float cb)
{
    auto p = std::make_shared<Primitive>(PrimitiveType::Quad);
    p->setName(id);
    p->setPosition({px, py, pz});
    p->setRotation({rx, ry, rz});
    p->setScale({sx, sy, sz});
    p->setColor({cr, cg, cb});
    return p;
}

std::shared_ptr<Scene> SceneBuilder::buildRoom() {
    auto scene = std::make_shared<Scene>("SmallRoom");

    scene->addPrimitive(makeQuad("floor", 0.0f, -1.5f, 0.0f, -90.0f, 0.0f, 0.0f, 6.0f, 6.0f, 1.0f, 0.4f, 0.3f, 0.2f));
    scene->addPrimitive(makeQuad("ceiling", 0.0f, 1.5f, 0.0f, 90.0f, 0.0f, 0.0f, 6.0f, 6.0f, 1.0f, 0.7f, 0.7f, 0.65f));
    scene->addPrimitive(makeQuad("wall_back", 0.0f, 0.0f, -3.0f, 0.0f, 0.0f, 0.0f, 6.0f, 3.0f, 1.0f, 0.6f, 0.55f, 0.5f));
    scene->addPrimitive(makeQuad("wall_front_lower", 0.0f, -0.75f, 3.0f, 0.0f, 180.0f, 0.0f, 6.0f, 1.5f, 1.0f, 0.6f, 0.55f, 0.5f));
    scene->addPrimitive(makeQuad("wall_front_upper", 0.0f, 0.75f, 3.0f, 0.0f, 180.0f, 0.0f, 6.0f, 1.5f, 1.0f, 0.6f, 0.55f, 0.5f));
    scene->addPrimitive(makeQuad("wall_front_left", -2.0f, 0.0f, 3.0f, 0.0f, 180.0f, 0.0f, 2.0f, 3.0f, 1.0f, 0.6f, 0.55f, 0.5f));
    scene->addPrimitive(makeQuad("wall_front_right", 2.0f, 0.0f, 3.0f, 0.0f, 180.0f, 0.0f, 2.0f, 3.0f, 1.0f, 0.6f, 0.55f, 0.5f));
    scene->addPrimitive(makeQuad("wall_left", -3.0f, 0.0f, 0.0f, 0.0f, 90.0f, 0.0f, 6.0f, 3.0f, 1.0f, 0.55f, 0.5f, 0.45f));
    scene->addPrimitive(makeQuad("wall_right_upper", 3.0f, 0.75f, 0.0f, 0.0f, -90.0f, 0.0f, 6.0f, 1.5f, 1.0f, 0.55f, 0.5f, 0.45f));
    scene->addPrimitive(makeQuad("wall_right_lower_left", 3.0f, -0.75f, -1.5f, 0.0f, -90.0f, 0.0f, 3.0f, 1.5f, 1.0f, 0.55f, 0.5f, 0.45f));
    scene->addPrimitive(makeQuad("wall_right_lower_right", 3.0f, -0.75f, 1.5f, 0.0f, -90.0f, 0.0f, 3.0f, 1.5f, 1.0f, 0.55f, 0.5f, 0.45f));

    auto camera = std::make_shared<Camera>(ProjectionType::Perspective);
    camera->setPosition({4.5f, 2.0f, 4.5f});
    camera->lookAt({0.0f, 0.0f, 0.0f});
    camera->setFOV(60.0f);
    camera->update();
    scene->addCamera(camera);

    return scene;
}

// Minimal JSON field extractor
static std::string jsonFind(const std::string& json, const std::string& key) {
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
}

static float jsonFindFloat(const std::string& json, const std::string& key, float def) {
    std::string val = jsonFind(json, key);
    return val.empty() ? def : std::stof(val);
}

static int jsonFindInt(const std::string& json, const std::string& key, int def) {
    std::string val = jsonFind(json, key);
    return val.empty() ? def : std::stoi(val);
}

static uint64_t jsonFindU64(const std::string& json, const std::string& key, uint64_t def) {
    std::string val = jsonFind(json, key);
    return val.empty() ? def : std::stoull(val);
}

// Extract a JSON array of 3 floats: "key": [x, y, z]
static void jsonFindVec3(const std::string& json, const std::string& key, float& x, float& y, float& z) {
    auto pos = json.find("\"" + key + "\"");
    if (pos == std::string::npos) return;
    pos = json.find('[', pos);
    if (pos == std::string::npos) return;
    pos++;
    auto end = json.find(']', pos);
    if (end == std::string::npos) return;
    std::string arr = json.substr(pos, end - pos);
    // Parse comma-separated values
    size_t p1 = arr.find(',');
    size_t p2 = (p1 == std::string::npos) ? std::string::npos : arr.find(',', p1 + 1);
    if (p1 != std::string::npos) x = std::stof(arr.substr(0, p1));
    if (p1 != std::string::npos && p2 != std::string::npos) y = std::stof(arr.substr(p1 + 1, p2 - p1 - 1));
    if (p2 != std::string::npos) {
        auto rest = arr.substr(p2 + 1);
        z = std::stof(rest);
    }
}

// Find all JSON objects matching a key pattern in an array
static std::vector<std::string> jsonFindArrayObjects(const std::string& json, const std::string& arrayKey) {
    std::vector<std::string> results;
    auto arrPos = json.find("\"" + arrayKey + "\"");
    if (arrPos == std::string::npos) return results;
    arrPos = json.find('[', arrPos);
    if (arrPos == std::string::npos) return results;
    arrPos++;

    while (arrPos < json.size()) {
        auto objStart = json.find('{', arrPos);
        if (objStart == std::string::npos) break;
        int depth = 1;
        size_t pos = objStart + 1;
        while (pos < json.size() && depth > 0) {
            if (json[pos] == '{') depth++;
            else if (json[pos] == '}') depth--;
            pos++;
        }
        results.push_back(json.substr(objStart, pos - objStart));
        arrPos = pos;
    }
    return results;
}

std::shared_ptr<Scene> SceneBuilder::importState(const std::string& path, ProjectManifest& manifest) {
    std::ifstream f(path);
    if (!f.is_open()) {
        std::cerr << "Failed to open project file: " << path << std::endl;
        return nullptr;
    }

    std::string content((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
    f.close();

    // Parse project manifest
    manifest.hscVersion = jsonFind(content, "hsc_version");
    manifest.engineVersion = jsonFind(content, "engine_version");
    manifest.projectName = jsonFind(content, "name");
    manifest.description = jsonFind(content, "description");
    manifest.sceneName = jsonFind(content, "scene_name");
    manifest.filePath = path;

    if (manifest.sceneName.empty()) manifest.sceneName = manifest.projectName;
    if (manifest.sceneName.empty()) manifest.sceneName = "LoadedScene";

    // Parse clear color from scene block
    auto sceneBlock = content;
    manifest.clearColorR = jsonFindFloat(sceneBlock, "clear_color_r", 0.05f);
    manifest.clearColorG = jsonFindFloat(sceneBlock, "clear_color_g", 0.05f);
    manifest.clearColorB = jsonFindFloat(sceneBlock, "clear_color_b", 0.08f);
    manifest.worldRevision = jsonFindU64(content, "world_revision", 0);

    auto scene = std::make_shared<Scene>(manifest.sceneName);

    // Parse objects
    auto objects = jsonFindArrayObjects(content, "objects");
    for (auto& objStr : objects) {
        std::string typeStr = jsonFind(objStr, "type");
        PrimitiveType ptype = PrimitiveType::Quad;
        if (typeStr == "Triangle" || typeStr == "triangle" || typeStr == "0") ptype = PrimitiveType::Triangle;
        else if (typeStr == "Quad" || typeStr == "quad" || typeStr == "1") ptype = PrimitiveType::Quad;
        else if (typeStr == "Cube" || typeStr == "cube" || typeStr == "2") ptype = PrimitiveType::Cube;
        else if (typeStr == "Sphere" || typeStr == "sphere" || typeStr == "3") ptype = PrimitiveType::Sphere;

        auto prim = std::make_shared<Primitive>(ptype);
        std::string name = jsonFind(objStr, "id");
        if (name.empty()) name = jsonFind(objStr, "name");
        if (!name.empty()) prim->setName(name);

        float px=0,py=0,pz=0, rx=0,ry=0,rz=0, sx=1,sy=1,sz=1, cr=1,cg=1,cb=1;
        jsonFindVec3(objStr, "position", px, py, pz);
        jsonFindVec3(objStr, "rotation", rx, ry, rz);
        jsonFindVec3(objStr, "scale", sx, sy, sz);
        jsonFindVec3(objStr, "color", cr, cg, cb);

        prim->setPosition({px, py, pz});
        prim->setRotation({rx, ry, rz});
        prim->setScale({sx, sy, sz});
        prim->setColor({cr, cg, cb});

        scene->addPrimitive(prim);
    }

    // Parse cameras
    auto cameras = jsonFindArrayObjects(content, "cameras");
    if (cameras.empty()) {
        // Try legacy single-camera format
        auto cam = std::make_shared<Camera>(ProjectionType::Perspective);
        float cx=5,cy=5,cz=5, tx=0,ty=0,tz=0, fov=60;
        // Try to find camera block
        auto camPos = content.find("\"camera\"");
        if (camPos != std::string::npos) {
            auto camBlock = content.substr(camPos, 500);
            jsonFindVec3(camBlock, "position", cx, cy, cz);
            jsonFindVec3(camBlock, "target", tx, ty, tz);
            fov = jsonFindFloat(camBlock, "fov", 60.0f);
        }
        cam->setPosition({cx, cy, cz});
        cam->lookAt({tx, ty, tz});
        cam->setFOV(fov);
        cam->update();
        scene->addCamera(cam);
    } else {
        for (auto& camStr : cameras) {
            auto cam = std::make_shared<Camera>(ProjectionType::Perspective);
            float cx=5,cy=5,cz=5, tx=0,ty=0,tz=0, fov=60;
            jsonFindVec3(camStr, "position", cx, cy, cz);
            jsonFindVec3(camStr, "target", tx, ty, tz);
            fov = jsonFindFloat(camStr, "fov", 60.0f);
            std::string projType = jsonFind(camStr, "type");
            if (projType == "Orthographic" || projType == "orthographic")
                cam = std::make_shared<Camera>(ProjectionType::Orthographic);
            cam->setPosition({cx, cy, cz});
            cam->lookAt({tx, ty, tz});
            cam->setFOV(fov);
            cam->update();
            scene->addCamera(cam);
        }
    }

    std::cout << "Project loaded: " << manifest.projectName
              << " (" << scene->getPrimitiveCount() << " objects, "
              << scene->getCameraCount() << " cameras, rev " << manifest.worldRevision << ")" << std::endl;

    return scene;
}

void SceneBuilder::exportState(const Scene& scene, const std::string& path) {
    std::ofstream f(path);
    if (!f.is_open()) {
        std::cerr << "Failed to open export file: " << path << std::endl;
        return;
    }
    f << "{\n";
    f << "  \"scene\": \"" << scene.getName() << "\",\n";
    f << "  \"primitive_count\": " << scene.getPrimitiveCount() << ",\n";
    f << "  \"camera_count\": " << scene.getCameraCount() << ",\n";
    f << "  \"objects\": [\n";

    auto& prims = scene.getPrimitives();
    for (size_t i = 0; i < prims.size(); i++) {
        auto& p = prims[i];
        f << "    {\n";
        f << "      \"index\": " << i << ",\n";
        f << "      \"type\": \"" << static_cast<int>(p->getType()) << "\",\n";
        f << "      \"position\": [" << p->getPosition().x << ", " << p->getPosition().y << ", " << p->getPosition().z << "],\n";
        f << "      \"rotation\": [" << p->getRotation().x << ", " << p->getRotation().y << ", " << p->getRotation().z << "],\n";
        f << "      \"scale\": [" << p->getScale().x << ", " << p->getScale().y << ", " << p->getScale().z << "],\n";
        f << "      \"color\": [" << p->getColor().x << ", " << p->getColor().y << ", " << p->getColor().z << "]\n";
        f << "    }" << (i < prims.size() - 1 ? "," : "") << "\n";
    }

    f << "  ],\n";
    f << "  \"camera\": {\n";
    auto cam = scene.getActiveCamera();
    if (cam) {
        f << "    \"position\": [" << cam->getPosition().x << ", " << cam->getPosition().y << ", " << cam->getPosition().z << "],\n";
        f << "    \"type\": \"" << (cam->getProjectionType() == ProjectionType::Perspective ? "Perspective" : "Orthographic") << "\"\n";
    }
    f << "  }\n";
    f << "}\n";
    f.close();
}

static std::string typeStr(PrimitiveType t) {
    switch (t) {
        case PrimitiveType::Triangle: return "Triangle";
        case PrimitiveType::Quad: return "Quad";
        case PrimitiveType::Cube: return "Cube";
        case PrimitiveType::Sphere: return "Sphere";
    }
    return "Quad";
}

void SceneBuilder::exportHSC(const Scene& scene, const Camera& camera, const ProjectManifest& manifest, const std::string& path) {
    std::ofstream f(path);
    if (!f.is_open()) {
        std::cerr << "Failed to write HSC file: " << path << std::endl;
        return;
    }

    f << "{\n";
    f << "  \"hsc_version\": \"" << manifest.hscVersion << "\",\n";
    f << "  \"engine_version\": \"" << manifest.engineVersion << "\",\n";
    f << "  \"world_revision\": " << manifest.worldRevision << ",\n";
    f << "  \"project\": {\n";
    f << "    \"name\": \"" << manifest.projectName << "\",\n";
    f << "    \"description\": \"" << manifest.description << "\"\n";
    f << "  },\n";
    f << "  \"scene\": {\n";
    f << "    \"name\": \"" << scene.getName() << "\",\n";
    f << "    \"clear_color_r\": " << manifest.clearColorR << ",\n";
    f << "    \"clear_color_g\": " << manifest.clearColorG << ",\n";
    f << "    \"clear_color_b\": " << manifest.clearColorB << ",\n";

    // Objects
    f << "    \"objects\": [\n";
    auto& prims = scene.getPrimitives();
    for (size_t i = 0; i < prims.size(); i++) {
        auto& p = prims[i];
        f << "      {\n";
        f << "        \"id\": \"" << p->getName() << "\",\n";
        f << "        \"type\": \"" << typeStr(p->getType()) << "\",\n";
        f << "        \"position\": [" << p->getPosition().x << ", " << p->getPosition().y << ", " << p->getPosition().z << "],\n";
        f << "        \"rotation\": [" << p->getRotation().x << ", " << p->getRotation().y << ", " << p->getRotation().z << "],\n";
        f << "        \"scale\": [" << p->getScale().x << ", " << p->getScale().y << ", " << p->getScale().z << "],\n";
        f << "        \"color\": [" << p->getColor().x << ", " << p->getColor().y << ", " << p->getColor().z << "]\n";
        f << "      }" << (i < prims.size() - 1 ? "," : "") << "\n";
    }
    f << "    ],\n";

    // Cameras
    f << "    \"cameras\": [\n";
    f << "      {\n";
    f << "        \"id\": \"main_camera\",\n";
    f << "        \"type\": \"" << (camera.getProjectionType() == ProjectionType::Perspective ? "Perspective" : "Orthographic") << "\",\n";
    f << "        \"position\": [" << camera.getPosition().x << ", " << camera.getPosition().y << ", " << camera.getPosition().z << "],\n";
    f << "        \"target\": [" << camera.getTarget().x << ", " << camera.getTarget().y << ", " << camera.getTarget().z << "],\n";
    f << "        \"fov\": " << camera.getFOV() << "\n";
    f << "      }\n";
    f << "    ]\n";
    f << "  }\n";
    f << "}\n";

    f.close();
    std::cout << "HSC project saved: " << path << " (rev " << manifest.worldRevision << ")" << std::endl;
}

bool SceneBuilder::createProject(const std::string& dir, const std::string& name) {
    // Create directory structure
    auto createDir = [](const std::string& path) {
#ifdef _WIN32
        CreateDirectoryA(path.c_str(), nullptr);
#else
        mkdir(path.c_str(), 0755);
#endif
    };

    createDir(dir);
    createDir(dir + "/scenes");
    createDir(dir + "/assets");
    createDir(dir + "/generated");
    createDir(dir + "/scripts");
    createDir(dir + "/state");
    createDir(dir + "/build");

    // Create initial .hsc file
    std::string hscPath = dir + "/" + name + ".hsc";
    std::ofstream f(hscPath);
    if (!f.is_open()) {
        std::cerr << "Failed to create project file: " << hscPath << std::endl;
        return false;
    }

    f << "{\n";
    f << "  \"hsc_version\": \"1.0\",\n";
    f << "  \"engine_version\": \"0.1.0\",\n";
    f << "  \"world_revision\": 0,\n";
    f << "  \"project\": {\n";
    f << "    \"name\": \"" << name << "\",\n";
    f << "    \"description\": \"New HSE project\"\n";
    f << "  },\n";
    f << "  \"scene\": {\n";
    f << "    \"name\": \"" << name << "\",\n";
    f << "    \"clear_color_r\": 0.05,\n";
    f << "    \"clear_color_g\": 0.05,\n";
    f << "    \"clear_color_b\": 0.08,\n";
    f << "    \"objects\": [\n";
    f << "      {\n";
    f << "        \"id\": \"floor\",\n";
    f << "        \"type\": \"Quad\",\n";
    f << "        \"position\": [0, -1.5, 0],\n";
    f << "        \"rotation\": [-90, 0, 0],\n";
    f << "        \"scale\": [8, 8, 1],\n";
    f << "        \"color\": [0.3, 0.25, 0.2]\n";
    f << "      },\n";
    f << "      {\n";
    f << "        \"id\": \"wall_back\",\n";
    f << "        \"type\": \"Quad\",\n";
    f << "        \"position\": [0, 0, -4],\n";
    f << "        \"rotation\": [0, 0, 0],\n";
    f << "        \"scale\": [8, 3, 1],\n";
    f << "        \"color\": [0.5, 0.5, 0.55]\n";
    f << "      },\n";
    f << "      {\n";
    f << "        \"id\": \"wall_left\",\n";
    f << "        \"type\": \"Quad\",\n";
    f << "        \"position\": [-4, 0, 0],\n";
    f << "        \"rotation\": [0, 90, 0],\n";
    f << "        \"scale\": [8, 3, 1],\n";
    f << "        \"color\": [0.55, 0.5, 0.5]\n";
    f << "      },\n";
    f << "      {\n";
    f << "        \"id\": \"wall_right\",\n";
    f << "        \"type\": \"Quad\",\n";
    f << "        \"position\": [4, 0, 0],\n";
    f << "        \"rotation\": [0, -90, 0],\n";
    f << "        \"scale\": [8, 3, 1],\n";
    f << "        \"color\": [0.55, 0.5, 0.5]\n";
    f << "      }\n";
    f << "    ],\n";
    f << "    \"cameras\": [\n";
    f << "      {\n";
    f << "        \"id\": \"main_camera\",\n";
    f << "        \"type\": \"Perspective\",\n";
    f << "        \"position\": [6, 4, 6],\n";
    f << "        \"target\": [0, 0, 0],\n";
    f << "        \"fov\": 60\n";
    f << "      }\n";
    f << "    ]\n";
    f << "  }\n";
    f << "}\n";

    f.close();
    std::cout << "Project created: " << hscPath << std::endl;
    return true;
}

} // namespace hse
