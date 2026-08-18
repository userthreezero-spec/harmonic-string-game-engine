#include "scene/scene_builder.h"
#include "scene/primitive.h"
#include "scene/camera.h"
#include <fstream>
#include <iostream>

namespace hse {

static std::shared_ptr<Primitive> makeQuad(
    const std::string& id,
    float px, float py, float pz,
    float rx, float ry, float rz,
    float sx, float sy, float sz,
    float cr, float cg, float cb)
{
    auto p = std::make_shared<Primitive>(PrimitiveType::Quad);
    p->setPosition({px, py, pz});
    p->setRotation({rx, ry, rz});
    p->setScale({sx, sy, sz});
    p->setColor({cr, cg, cb});
    return p;
}

std::shared_ptr<Scene> SceneBuilder::buildRoom() {
    auto scene = std::make_shared<Scene>("SmallRoom");

    // Floor (brown)
    scene->addPrimitive(makeQuad("floor",
        0.0f, -1.5f, 0.0f,
        -90.0f, 0.0f, 0.0f,
        6.0f, 6.0f, 1.0f,
        0.4f, 0.3f, 0.2f));

    // Ceiling (light)
    scene->addPrimitive(makeQuad("ceiling",
        0.0f, 1.5f, 0.0f,
        90.0f, 0.0f, 0.0f,
        6.0f, 6.0f, 1.0f,
        0.7f, 0.7f, 0.65f));

    // Back wall (solid)
    scene->addPrimitive(makeQuad("wall_back",
        0.0f, 0.0f, -3.0f,
        0.0f, 0.0f, 0.0f,
        6.0f, 3.0f, 1.0f,
        0.6f, 0.55f, 0.5f));

    // Front wall - 4 pieces around window opening
    scene->addPrimitive(makeQuad("wall_front_lower",
        0.0f, -0.75f, 3.0f,
        0.0f, 180.0f, 0.0f,
        6.0f, 1.5f, 1.0f,
        0.6f, 0.55f, 0.5f));

    scene->addPrimitive(makeQuad("wall_front_upper",
        0.0f, 0.75f, 3.0f,
        0.0f, 180.0f, 0.0f,
        6.0f, 1.5f, 1.0f,
        0.6f, 0.55f, 0.5f));

    scene->addPrimitive(makeQuad("wall_front_left",
        -2.0f, 0.0f, 3.0f,
        0.0f, 180.0f, 0.0f,
        2.0f, 3.0f, 1.0f,
        0.6f, 0.55f, 0.5f));

    scene->addPrimitive(makeQuad("wall_front_right",
        2.0f, 0.0f, 3.0f,
        0.0f, 180.0f, 0.0f,
        2.0f, 3.0f, 1.0f,
        0.6f, 0.55f, 0.5f));

    // Left wall (solid)
    scene->addPrimitive(makeQuad("wall_left",
        -3.0f, 0.0f, 0.0f,
        0.0f, 90.0f, 0.0f,
        6.0f, 3.0f, 1.0f,
        0.55f, 0.5f, 0.45f));

    // Right wall - 3 pieces around doorway
    scene->addPrimitive(makeQuad("wall_right_upper",
        3.0f, 0.75f, 0.0f,
        0.0f, -90.0f, 0.0f,
        6.0f, 1.5f, 1.0f,
        0.55f, 0.5f, 0.45f));

    scene->addPrimitive(makeQuad("wall_right_lower_left",
        3.0f, -0.75f, -1.5f,
        0.0f, -90.0f, 0.0f,
        3.0f, 1.5f, 1.0f,
        0.55f, 0.5f, 0.45f));

    scene->addPrimitive(makeQuad("wall_right_lower_right",
        3.0f, -0.75f, 1.5f,
        0.0f, -90.0f, 0.0f,
        3.0f, 1.5f, 1.0f,
        0.55f, 0.5f, 0.45f));

    // Camera - positioned outside room, looking through doorway
    auto camera = std::make_shared<Camera>(ProjectionType::Perspective);
    camera->setPosition({4.5f, 2.0f, 4.5f});
    camera->lookAt({0.0f, 0.0f, 0.0f});
    camera->setFOV(60.0f);
    camera->update();
    scene->addCamera(camera);

    std::cout << "Scene built: " << scene->getPrimitiveCount() << " primitives, "
              << scene->getCameraCount() << " cameras" << std::endl;

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
    std::cout << "Scene state exported to: " << path << std::endl;
}

} // namespace hse
