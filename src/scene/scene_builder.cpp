#include "scene/scene_builder.h"
#include "scene/primitive.h"
#include "scene/camera.h"
#include "renderer/material.h"
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
    std::shared_ptr<Material> material)
{
    auto p = std::make_shared<Primitive>(PrimitiveType::Quad);
    p->setName(id);
    p->setPosition({px, py, pz});
    p->setRotation({rx, ry, rz});
    p->setScale({sx, sy, sz});
    p->setMaterial(material);
    return p;
}

std::shared_ptr<Scene> SceneBuilder::buildRoom() {
    auto scene = std::make_shared<Scene>("SmallRoom");

    auto matFloor = std::make_shared<Material>("mat_floor");
    matFloor->setAlbedo({0.4f, 0.3f, 0.2f});
    matFloor->setRoughness(0.8f);
    scene->addMaterial(matFloor);

    auto matWall = std::make_shared<Material>("mat_wall");
    matWall->setAlbedo({0.6f, 0.55f, 0.5f});
    matWall->setRoughness(0.5f);
    scene->addMaterial(matWall);

    auto matCeiling = std::make_shared<Material>("mat_ceiling");
    matCeiling->setAlbedo({0.7f, 0.7f, 0.65f});
    matCeiling->setRoughness(0.9f);
    scene->addMaterial(matCeiling);

    scene->addPrimitive(makeQuad("floor", 0.0f, -1.5f, 0.0f, -90.0f, 0.0f, 0.0f, 6.0f, 6.0f, 1.0f, matFloor));
    scene->addPrimitive(makeQuad("ceiling", 0.0f, 1.5f, 0.0f, 90.0f, 0.0f, 0.0f, 6.0f, 6.0f, 1.0f, matCeiling));
    scene->addPrimitive(makeQuad("wall_back", 0.0f, 0.0f, -3.0f, 0.0f, 0.0f, 0.0f, 6.0f, 3.0f, 1.0f, matWall));
    scene->addPrimitive(makeQuad("wall_front_lower", 0.0f, -0.75f, 3.0f, 0.0f, 180.0f, 0.0f, 6.0f, 1.5f, 1.0f, matWall));
    scene->addPrimitive(makeQuad("wall_front_upper", 0.0f, 0.75f, 3.0f, 0.0f, 180.0f, 0.0f, 6.0f, 1.5f, 1.0f, matWall));
    scene->addPrimitive(makeQuad("wall_front_left", -2.0f, 0.0f, 3.0f, 0.0f, 180.0f, 0.0f, 2.0f, 3.0f, 1.0f, matWall));
    scene->addPrimitive(makeQuad("wall_front_right", 2.0f, 0.0f, 3.0f, 0.0f, 180.0f, 0.0f, 2.0f, 3.0f, 1.0f, matWall));
    scene->addPrimitive(makeQuad("wall_left", -3.0f, 0.0f, 0.0f, 0.0f, 90.0f, 0.0f, 6.0f, 3.0f, 1.0f, matWall));
    scene->addPrimitive(makeQuad("wall_right_upper", 3.0f, 0.75f, 0.0f, 0.0f, -90.0f, 0.0f, 6.0f, 1.5f, 1.0f, matWall));
    scene->addPrimitive(makeQuad("wall_right_lower_left", 3.0f, -0.75f, -1.5f, 0.0f, -90.0f, 0.0f, 3.0f, 1.5f, 1.0f, matWall));
    scene->addPrimitive(makeQuad("wall_right_lower_right", 3.0f, -0.75f, 1.5f, 0.0f, -90.0f, 0.0f, 3.0f, 1.5f, 1.0f, matWall));

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
    while (pos < json.size() && (json[pos] == ' ' || json[pos] == '\n' || json[pos] == '\r')) pos++;
    if (pos >= json.size()) return "";
    if (json[pos] == '"') {
        pos++;
        auto end = json.find('"', pos);
        if (end == std::string::npos) return "";
        return json.substr(pos, end - pos);
    }
    auto end = json.find_first_of(",}\n", pos);
    if (end == std::string::npos) return json.substr(pos);
    return json.substr(pos, end - pos);
}

static float jsonFindFloat(const std::string& json, const std::string& key, float def) {
    std::string val = jsonFind(json, key);
    return val.empty() ? def : std::stof(val);
}

static void jsonFindVec3(const std::string& json, const std::string& key, float& x, float& y, float& z) {
    auto pos = json.find("\"" + key + "\"");
    if (pos == std::string::npos) return;
    pos = json.find('[', pos);
    if (pos == std::string::npos) return;
    pos++;
    auto end = json.find(']', pos);
    if (end == std::string::npos) return;
    std::string arr = json.substr(pos, end - pos);
    size_t p1 = arr.find(',');
    size_t p2 = (p1 == std::string::npos) ? std::string::npos : arr.find(',', p1 + 1);
    if (p1 != std::string::npos) x = std::stof(arr.substr(0, p1));
    if (p1 != std::string::npos && p2 != std::string::npos) y = std::stof(arr.substr(p1 + 1, p2 - p1 - 1));
    if (p2 != std::string::npos) {
        auto rest = arr.substr(p2 + 1);
        z = std::stof(rest);
    }
}

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
    if (!f.is_open()) return nullptr;
    std::string content((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
    f.close();

    manifest.projectName = jsonFind(content, "name");
    auto scene = std::make_shared<Scene>(jsonFind(content, "scene_name"));

    // Parse Materials
    auto materialsJson = jsonFindArrayObjects(content, "materials");
    for (auto& mStr : materialsJson) {
        std::string name = jsonFind(mStr, "id");
        auto mat = std::make_shared<Material>(name);
        float r=1, g=1, b=1;
        jsonFindVec3(mStr, "albedo", r, g, b);
        mat->setAlbedo({r, g, b});
        mat->setRoughness(jsonFindFloat(mStr, "roughness", 0.5f));
        mat->setMetallic(jsonFindFloat(mStr, "metallic", 0.0f));

        std::string texPath = jsonFind(mStr, "albedo_map");
        if (!texPath.empty()) {
            mat->setAlbedoMap(std::make_shared<Texture>(texPath));
        }
        scene->addMaterial(mat);
    }

    // Parse Objects
    auto objects = jsonFindArrayObjects(content, "objects");
    std::vector<std::pair<std::shared_ptr<Primitive>, std::string>> parentRefs;
    for (auto& objStr : objects) {
        std::string typeStr = jsonFind(objStr, "type");
        PrimitiveType ptype = PrimitiveType::Quad;
        if (typeStr == "Triangle") ptype = PrimitiveType::Triangle;
        else if (typeStr == "Cube") ptype = PrimitiveType::Cube;

        auto prim = std::make_shared<Primitive>(ptype);
        prim->setName(jsonFind(objStr, "id"));

        float px=0,py=0,pz=0, rx=0,ry=0,rz=0, sx=1,sy=1,sz=1;
        jsonFindVec3(objStr, "position", px, py, pz);
        jsonFindVec3(objStr, "rotation", rx, ry, rz);
        jsonFindVec3(objStr, "scale", sx, sy, sz);

        prim->setPosition({px, py, pz});
        prim->setRotation({rx, ry, rz});
        prim->setScale({sx, sy, sz});

        std::string matId = jsonFind(objStr, "material");
        auto mat = scene->getMaterial(matId);
        if (mat) prim->setMaterial(mat);

        std::string parentName = jsonFind(objStr, "parent");
        if (!parentName.empty()) {
            parentRefs.push_back({prim, parentName});
        }

        scene->addPrimitive(prim);
    }

    // Establish parent-child relationships
    for (auto& [child, parentName] : parentRefs) {
        auto parent = scene->findByName(parentName);
        if (parent) {
            child->setParent(parent);
        }
    }

    return scene;
}

void SceneBuilder::exportHSC(const Scene& scene, const Camera& camera, const ProjectManifest& manifest, const std::string& path) {
    std::ofstream f(path);
    if (!f.is_open()) return;

    f << "{\n";
    f << "  \"name\": \"" << manifest.projectName << "\",\n";
    f << "  \"materials\": [\n";
    auto& mats = scene.getMaterials();
    size_t mCount = 0;
    for (auto const& [name, mat] : mats) {
        f << "    {\n";
        f << "      \"id\": \"" << mat->getName() << "\",\n";
        f << "      \"albedo\": [" << mat->getAlbedo().x << "," << mat->getAlbedo().y << "," << mat->getAlbedo().z << "],\n";
        f << "      \"roughness\": " << mat->getRoughness() << ",\n";
        f << "      \"metallic\": " << mat->getMetallic() << "";
        if (mat->hasAlbedoMap()) {
            f << ",\n      \"albedo_map\": \"assets/textures/logo.png\""; // TODO: store real path
        }
        f << "\n    }" << (++mCount < mats.size() ? "," : "") << "\n";
    }
    f << "  ],\n";
    f << "  \"objects\": [\n";
    auto& prims = scene.getPrimitives();
    for (size_t i = 0; i < prims.size(); i++) {
        auto& p = prims[i];
        f << "    {\n";
        f << "      \"id\": \"" << p->getName() << "\",\n";
        f << "      \"type\": \"" << (p->getType() == PrimitiveType::Triangle ? "Triangle" : (p->getType() == PrimitiveType::Cube ? "Cube" : "Quad")) << "\",\n";
        f << "      \"position\": [" << p->getPosition().x << "," << p->getPosition().y << "," << p->getPosition().z << "],\n";
        f << "      \"material\": \"" << (p->getMaterial() ? p->getMaterial()->getName() : "Default") << "\"";
        auto parentHsc = p->getParent();
        if (parentHsc) {
            f << ",\n      \"parent\": \"" << parentHsc->getName() << "\"";
        }
        f << "\n    }" << (i < prims.size() - 1 ? "," : "") << "\n";
    }
    f << "  ]\n";
    f << "}\n";
    f.close();
}

void SceneBuilder::exportState(const Scene& scene, const std::string& path) {
    std::ofstream f(path);
    if (!f.is_open()) return;

    f << "{\n";
    f << "  \"scene_name\": \"" << scene.getName() << "\",\n";
    f << "  \"materials\": [\n";
    auto& mats = scene.getMaterials();
    size_t mCount = 0;
    for (auto const& [name, mat] : mats) {
        f << "    {\n";
        f << "      \"id\": \"" << mat->getName() << "\",\n";
        f << "      \"albedo\": [" << mat->getAlbedo().x << "," << mat->getAlbedo().y << "," << mat->getAlbedo().z << "],\n";
        f << "      \"roughness\": " << mat->getRoughness() << ",\n";
        f << "      \"metallic\": " << mat->getMetallic() << "\n";
        f << "    }" << (++mCount < mats.size() ? "," : "") << "\n";
    }
    f << "  ],\n";
    f << "  \"objects\": [\n";
    auto& prims = scene.getPrimitives();
    for (size_t i = 0; i < prims.size(); i++) {
        auto& p = prims[i];
        f << "    {\n";
        f << "      \"id\": \"" << p->getName() << "\",\n";
        f << "      \"type\": \"" << (p->getType() == PrimitiveType::Triangle ? "Triangle" : (p->getType() == PrimitiveType::Cube ? "Cube" : "Quad")) << "\",\n";
        f << "      \"position\": [" << p->getPosition().x << "," << p->getPosition().y << "," << p->getPosition().z << "],\n";
        f << "      \"material\": \"" << (p->getMaterial() ? p->getMaterial()->getName() : "Default") << "\"";
        auto parentState = p->getParent();
        if (parentState) {
            f << ",\n      \"parent\": \"" << parentState->getName() << "\"";
        }
        f << "\n    }" << (i < prims.size() - 1 ? "," : "") << "\n";
    }
    f << "  ]\n";
    f << "}\n";
    f.close();
}

bool SceneBuilder::createProject(const std::string& dir, const std::string& name) {
    return false; // Stub for now
}

} // namespace hse
