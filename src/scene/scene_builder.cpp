#include "scene/scene_builder.h"
#include "scene/primitive.h"
#include "scene/camera.h"
#include "scene/light.h"
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

    // Reconstruct Furniture as Assemblies
    auto matWood = std::make_shared<Material>("mat_wood");
    matWood->setAlbedo({0.3f, 0.2f, 0.1f});
    scene->addMaterial(matWood);

    auto tableGroup = std::make_shared<Primitive>(PrimitiveType::Group);
    tableGroup->setName("table");
    tableGroup->setPosition({1.5f, -0.7f, -1.8f}); // HSE-022 Window-side corner
    scene->addPrimitive(tableGroup);

    auto tableTop = std::make_shared<Primitive>(PrimitiveType::Cube);
    tableTop->setName("table_top");
    tableTop->setPosition({0, 0, 0}); // Local to group
    tableTop->setScale({2, 0.1, 1.2});
    tableTop->setMaterial(matWood);
    tableTop->setParent(tableGroup);
    scene->addPrimitive(tableTop);

    for (int i = 0; i < 4; i++) {
        auto leg = std::make_shared<Primitive>(PrimitiveType::Cube);
        leg->setName("table_leg_" + std::to_string(i+1));
        float lx = (i < 2) ? -0.9f : 0.9f;
        float lz = (i % 2 == 0) ? -0.5f : 0.5f;
        leg->setPosition({lx, -0.4f, lz});
        leg->setScale({0.1, 0.8, 0.1});
        leg->setMaterial(matWood);
        leg->setParent(tableGroup);
        scene->addPrimitive(leg);
    }

    auto chair1Group = std::make_shared<Primitive>(PrimitiveType::Group);
    chair1Group->setName("chair_1_assembly");
    chair1Group->setPosition({1.5f, -1.0f, -1.1f});
    chair1Group->setRotation({0, 180, 0});
    scene->addPrimitive(chair1Group);

    auto chair1Seat = std::make_shared<Primitive>(PrimitiveType::Cube);
    chair1Seat->setName("chair_1");
    chair1Seat->setPosition({0, 0, 0});
    chair1Seat->setScale({0.6, 0.1, 0.6});
    chair1Seat->setParent(chair1Group);
    scene->addPrimitive(chair1Seat);

    auto chair1Back = std::make_shared<Primitive>(PrimitiveType::Cube);
    chair1Back->setName("chair_1_back");
    chair1Back->setPosition({0, 0.4f, 0.3f});
    chair1Back->setScale({0.6, 0.8, 0.1});
    chair1Back->setParent(chair1Group);
    scene->addPrimitive(chair1Back);

    auto chair2Group = std::make_shared<Primitive>(PrimitiveType::Group);
    chair2Group->setName("chair_2_assembly");
    chair2Group->setPosition({1.5f, -1.0f, -2.5f});
    chair2Group->setRotation({0, 0, 0});
    scene->addPrimitive(chair2Group);

    auto chair2Seat = std::make_shared<Primitive>(PrimitiveType::Cube);
    chair2Seat->setName("chair_2");
    chair2Seat->setPosition({0, 0, 0});
    chair2Seat->setScale({0.6, 0.1, 0.6});
    chair2Seat->setParent(chair2Group);
    scene->addPrimitive(chair2Seat);

    auto chair2Back = std::make_shared<Primitive>(PrimitiveType::Cube);
    chair2Back->setName("chair_2_back");
    chair2Back->setPosition({0, 0.4f, 0.3f});
    chair2Back->setScale({0.6, 0.8, 0.1});
    chair2Back->setParent(chair2Group);
    scene->addPrimitive(chair2Back);

    auto mainLight = std::make_shared<Light>("main_light");
    mainLight->setPosition({0, 1.2f, 0});
    mainLight->setColor({1.0f, 0.9f, 0.8f});
    mainLight->setIntensity(1.5f);
    scene->addLight(mainLight);

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
    std::string search = "\"" + key + "\"";
    size_t pos = 0;
    while ((pos = json.find(search, pos)) != std::string::npos) {
        size_t colonPos = json.find(':', pos + search.length());
        if (colonPos == std::string::npos) {
            pos += search.length();
            continue;
        }

        bool onlySpace = true;
        for (size_t i = pos + search.length(); i < colonPos; i++) {
            if (!std::isspace(static_cast<unsigned char>(json[i]))) {
                onlySpace = false;
                break;
            }
        }

        if (!onlySpace) {
            pos += search.length();
            continue;
        }

        pos = colonPos + 1;
        while (pos < json.size() && std::isspace(static_cast<unsigned char>(json[pos]))) pos++;
        if (pos >= json.size()) return "";

        if (json[pos] == '"') {
            pos++;
            auto end = json.find('"', pos);
            if (end == std::string::npos) return "";
            return json.substr(pos, end - pos);
        }
        if (json[pos] == '{') {
            int depth = 0;
            for (size_t i = pos; i < json.size(); i++) {
                if (json[i] == '{') depth++;
                else if (json[i] == '}') depth--;
                if (depth == 0) return json.substr(pos, i - pos + 1);
            }
        }
        auto end = json.find_first_of(",}\n", pos);
        if (end == std::string::npos) return json.substr(pos);
        return json.substr(pos, end - pos);
    }
    return "";
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

    size_t arrEnd = json.size();
    int arrDepth = 1;
    for (size_t i = arrPos + 1; i < json.size(); i++) {
        if (json[i] == '[') arrDepth++;
        else if (json[i] == ']') arrDepth--;
        if (arrDepth == 0) {
            arrEnd = i;
            break;
        }
    }

    arrPos++;
    while (arrPos < arrEnd) {
        auto objStart = json.find('{', arrPos);
        if (objStart == std::string::npos || objStart >= arrEnd) break;
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
        else if (typeStr == "Group") ptype = PrimitiveType::Group;

        auto prim = std::make_shared<Primitive>(ptype);
        prim->setName(jsonFind(objStr, "id"));

        float px=0,py=0,pz=0, rx=0,ry=0,rz=0, sx=1,sy=1,sz=1;
        jsonFindVec3(objStr, "position", px, py, pz);
        jsonFindVec3(objStr, "rotation", rx, ry, rz);
        jsonFindVec3(objStr, "scale", sx, sy, sz);

        prim->setPosition({px, py, pz});
        prim->setRotation({rx, ry, rz});
        prim->setScale({sx, sy, sz});

        float rotSpeedX=0, rotSpeedY=0, rotSpeedZ=0;
        jsonFindVec3(objStr, "rotation_speed", rotSpeedX, rotSpeedY, rotSpeedZ);
        prim->setRotationSpeed({rotSpeedX, rotSpeedY, rotSpeedZ});

        std::string matId = jsonFind(objStr, "material");
        auto mat = scene->getMaterial(matId);
        if (mat) prim->setMaterial(mat);

        std::string parentName = jsonFind(objStr, "parent");
        if (!parentName.empty()) {
            parentRefs.push_back({prim, parentName});
        }

        scene->addPrimitive(prim);
    }

    // Parse Lights
    auto lights = jsonFindArrayObjects(content, "lights");
    for (auto& lightStr : lights) {
        auto light = std::make_shared<Light>(jsonFind(lightStr, "id"));
        float px=0,py=0,pz=0, cr=1,cg=1,cb=1;
        jsonFindVec3(lightStr, "position", px, py, pz);
        jsonFindVec3(lightStr, "color", cr, cg, cb);
        light->setPosition({px, py, pz});
        light->setColor({cr, cg, cb});
        light->setIntensity(jsonFindFloat(lightStr, "intensity", 1.0f));
        scene->addLight(light);
    }

    // Parse Camera
    auto cameraObj = jsonFind(content, "camera");
    if (!cameraObj.empty()) {
        auto camera = std::make_shared<Camera>();
        float px=0,py=0,pz=0, tx=0,ty=0,tz=0;
        jsonFindVec3(cameraObj, "position", px, py, pz);
        jsonFindVec3(cameraObj, "target", tx, ty, tz);
        camera->setPosition({px, py, pz});
        camera->lookAt({tx, ty, tz});
        camera->setFOV(jsonFindFloat(cameraObj, "fov", 60.0f));
        camera->update();
        scene->addCamera(camera);
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
    f << "  \"lights\": [\n";
    auto& lights = scene.getLights();
    for (size_t i = 0; i < lights.size(); i++) {
        auto& l = lights[i];
        f << "    {\n";
        f << "      \"id\": \"" << l->getName() << "\",\n";
        f << "      \"position\": [" << l->getPosition().x << "," << l->getPosition().y << "," << l->getPosition().z << "],\n";
        f << "      \"color\": [" << l->getColor().x << "," << l->getColor().y << "," << l->getColor().z << "],\n";
        f << "      \"intensity\": " << l->getIntensity() << "\n";
        f << "    }" << (i < lights.size() - 1 ? "," : "") << "\n";
    }
    f << "  ],\n";
    f << "  \"camera\": {\n";
    f << "    \"position\": [" << camera.getPosition().x << "," << camera.getPosition().y << "," << camera.getPosition().z << "],\n";
    f << "    \"target\": [" << camera.getTarget().x << "," << camera.getTarget().y << "," << camera.getTarget().z << "],\n";
    f << "    \"fov\": " << camera.getFOV() << "\n";
    f << "  },\n";
    f << "  \"objects\": [\n";
    auto& prims = scene.getPrimitives();
    for (size_t i = 0; i < prims.size(); i++) {
        auto& p = prims[i];
        f << "    {\n";
        f << "      \"id\": \"" << p->getName() << "\",\n";
        f << "      \"type\": \"" << (p->getType() == PrimitiveType::Triangle ? "Triangle" : (p->getType() == PrimitiveType::Cube ? "Cube" : (p->getType() == PrimitiveType::Group ? "Group" : "Quad"))) << "\",\n";
        f << "      \"position\": [" << p->getPosition().x << "," << p->getPosition().y << "," << p->getPosition().z << "],\n";
        f << "      \"rotation\": [" << p->getRotation().x << "," << p->getRotation().y << "," << p->getRotation().z << "],\n";
        f << "      \"scale\": [" << p->getScale().x << "," << p->getScale().y << "," << p->getScale().z << "],\n";
        f << "      \"rotation_speed\": [" << p->getRotationSpeed().x << "," << p->getRotationSpeed().y << "," << p->getRotationSpeed().z << "],\n";
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
    f << "  \"lights\": [\n";
    auto& lights = scene.getLights();
    for (size_t i = 0; i < lights.size(); i++) {
        auto& l = lights[i];
        f << "    {\n";
        f << "      \"id\": \"" << l->getName() << "\",\n";
        f << "      \"position\": [" << l->getPosition().x << "," << l->getPosition().y << "," << l->getPosition().z << "],\n";
        f << "      \"color\": [" << l->getColor().x << "," << l->getColor().y << "," << l->getColor().z << "],\n";
        f << "      \"intensity\": " << l->getIntensity() << "\n";
        f << "    }" << (i < lights.size() - 1 ? "," : "") << "\n";
    }
    f << "  ],\n";
    f << "  \"objects\": [\n";
    auto& prims = scene.getPrimitives();
    for (size_t i = 0; i < prims.size(); i++) {
        auto& p = prims[i];
        f << "    {\n";
        f << "      \"id\": \"" << p->getName() << "\",\n";
        f << "      \"type\": \"" << (p->getType() == PrimitiveType::Triangle ? "Triangle" : (p->getType() == PrimitiveType::Cube ? "Cube" : (p->getType() == PrimitiveType::Group ? "Group" : "Quad"))) << "\",\n";
        f << "      \"position\": [" << p->getPosition().x << "," << p->getPosition().y << "," << p->getPosition().z << "],\n";
        f << "      \"rotation\": [" << p->getRotation().x << "," << p->getRotation().y << "," << p->getRotation().z << "],\n";
        f << "      \"scale\": [" << p->getScale().x << "," << p->getScale().y << "," << p->getScale().z << "],\n";
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
