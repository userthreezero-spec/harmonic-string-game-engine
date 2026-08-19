#include "scene/scene.h"
#include "scene/camera.h"
#include "scene/primitive.h"
#include "scene/light.h"
#include "renderer/material.h"

namespace hse {

Scene::Scene(const std::string& name) : m_name(name) {}
Scene::~Scene() = default;

void Scene::addPrimitive(std::shared_ptr<Primitive> primitive) {
    size_t idx = m_primitives.size();
    m_primitives.push_back(primitive);
    m_idIndex[primitive->getID()] = idx;
}

void Scene::removePrimitive(int index) {
    if (index >= 0 && index < static_cast<int>(m_primitives.size())) {
        m_primitives.erase(m_primitives.begin() + index);
        rebuildIDIndex();
    }
}

bool Scene::removePrimitiveByID(uint64_t id) {
    auto it = m_idIndex.find(id);
    if (it == m_idIndex.end()) return false;
    size_t idx = it->second;
    if (idx >= m_primitives.size()) return false;
    m_primitives.erase(m_primitives.begin() + idx);
    rebuildIDIndex();
    return true;
}

std::shared_ptr<Primitive> Scene::findByID(uint64_t id) const {
    auto it = m_idIndex.find(id);
    if (it == m_idIndex.end()) return nullptr;
    if (it->second >= m_primitives.size()) return nullptr;
    return m_primitives[it->second];
}

std::shared_ptr<Primitive> Scene::findByName(const std::string& name) const {
    for (auto& p : m_primitives) {
        if (p->getName() == name) return p;
    }
    return nullptr;
}

int Scene::findIndexByID(uint64_t id) const {
    auto it = m_idIndex.find(id);
    if (it == m_idIndex.end()) return -1;
    return static_cast<int>(it->second);
}

void Scene::rebuildIDIndex() {
    m_idIndex.clear();
    for (size_t i = 0; i < m_primitives.size(); i++) {
        m_idIndex[m_primitives[i]->getID()] = i;
    }
}

void Scene::addCamera(std::shared_ptr<Camera> camera) {
    m_cameras.push_back(camera);
    if (m_cameras.size() == 1) {
        m_activeCameraIndex = 0;
    }
}

void Scene::setActiveCamera(int index) {
    if (index >= 0 && index < static_cast<int>(m_cameras.size())) {
        m_activeCameraIndex = index;
    }
}

std::shared_ptr<Camera> Scene::getActiveCamera() const {
    if (m_cameras.empty()) return nullptr;
    return m_cameras[m_activeCameraIndex];
}

void Scene::addLight(std::shared_ptr<Light> light) {
    if (light) m_lights.push_back(light);
}

void Scene::removeLight(int index) {
    if (index >= 0 && index < static_cast<int>(m_lights.size())) {
        m_lights.erase(m_lights.begin() + index);
    }
}

void Scene::addMaterial(std::shared_ptr<Material> material) {
    if (material) {
        m_materials[material->getName()] = material;
    }
}

std::shared_ptr<Material> Scene::getMaterial(const std::string& name) const {
    auto it = m_materials.find(name);
    if (it != m_materials.end()) return it->second;
    return nullptr;
}

bool Scene::reparent(uint64_t childID, uint64_t parentID) {
    if (childID == parentID) return false;
    auto child = findByID(childID);
    auto parent = findByID(parentID);
    if (!child) return false;
    if (parentID != 0 && !parent) return false;
    child->setParent(parent);
    return true;
}

std::vector<std::shared_ptr<Primitive>> Scene::getRoots() const {
    std::vector<std::shared_ptr<Primitive>> roots;
    for (auto& p : m_primitives) {
        if (!p->getParent()) {
            roots.push_back(p);
        }
    }
    return roots;
}

void Scene::computeAllWorldMatrices() {
    auto roots = getRoots();
    for (auto& root : roots) {
        root->computeWorldMatrix();
    }
}

void Scene::update(float deltaTime) {
    for (auto& prim : m_primitives) {
        Vec3 rot = prim->getRotation();
        Vec3 speed = prim->getRotationSpeed();
        rot.x += speed.x * deltaTime;
        rot.y += speed.y * deltaTime;
        rot.z += speed.z * deltaTime;
        prim->setRotation(rot);
    }

    auto cam = getActiveCamera();
    if (cam) {
        cam->updateOrbit(deltaTime);
    }
}

} // namespace hse
