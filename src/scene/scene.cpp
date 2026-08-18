#include "scene/scene.h"
#include "scene/primitive.h"

namespace hse {

Scene::Scene(const std::string& name) : m_name(name) {}
Scene::~Scene() = default;

void Scene::addPrimitive(std::shared_ptr<Primitive> primitive) {
    m_primitives.push_back(primitive);
}

void Scene::removePrimitive(int index) {
    if (index >= 0 && index < static_cast<int>(m_primitives.size())) {
        m_primitives.erase(m_primitives.begin() + index);
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

void Scene::update(float deltaTime) {
    for (auto& prim : m_primitives) {
        Vec3 rot = prim->getRotation();
        Vec3 speed = prim->getRotationSpeed();
        rot.x += speed.x * deltaTime;
        rot.y += speed.y * deltaTime;
        rot.z += speed.z * deltaTime;
        prim->setRotation(rot);
    }
}

} // namespace hse
