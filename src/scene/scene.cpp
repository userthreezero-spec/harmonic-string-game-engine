#include "scene/scene.h"
#include "scene/light.h"
#include "scene/primitive.h"
#include "scene/camera.h"
#include "scene/animated_light.h"
#include "scene/animated_primitive.h"
#include "scene/timeline.h"

namespace hse {

Scene::Scene(const std::string& name) : m_name(name) {}
Scene::~Scene() = default;

void Scene::computeAllWorldMatrices() {
    for (auto& p : m_primitives) {
        p->updateWorldTransform();
    }
}

void Scene::addPrimitive(std::shared_ptr<Primitive> primitive) {
    m_primitives.push_back(primitive);
}

void Scene::removePrimitive(int index) {
    if (index >= 0 && index < static_cast<int>(m_primitives.size())) {
        m_primitives.erase(m_primitives.begin() + index);
    }
}

void Scene::removePrimitiveByID(uint64_t id) {
    for (auto it = m_primitives.begin(); it != m_primitives.end(); ++it) {
        if ((*it)->getID() == id) {
            m_primitives.erase(it);
            break;
        }
    }
}

std::vector<uint64_t> Scene::getCollisions(uint64_t id) {
    std::vector<uint64_t> result;
    auto target = findByID(id);
    if (!target) return result;
    target->updateWorldTransform();
    BoundingBox targetBB = target->getBoundingBox();
    for (const auto& p : m_primitives) {
        if (p->getID() != id) {
            p->updateWorldTransform();
            if (p->getBoundingBox().intersects(targetBB)) {
                result.push_back(p->getID());
            }
        }
    }
    return result;
}

bool Scene::checkCollision(uint64_t id1, uint64_t id2) {
    auto p1 = findByID(id1);
    auto p2 = findByID(id2);
    if (!p1 || !p2) return false;
    p1->updateWorldTransform();
    p2->updateWorldTransform();
    return p1->getBoundingBox().intersects(p2->getBoundingBox());
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

void Scene::addLight(const Light& light) {
    m_lights.push_back(light);
}

void Scene::removeLight(int index) {
    if (index >= 0 && index < static_cast<int>(m_lights.size())) {
        m_lights.erase(m_lights.begin() + index);
    }
}

Light& Scene::getLight(int index) {
    return m_lights[index];
}

void Scene::addAnimatedLight(const AnimatedLight& al) {
    m_animatedLights.push_back(al);
}

void Scene::removeAnimatedLight(int index) {
    if (index >= 0 && index < static_cast<int>(m_animatedLights.size())) {
        m_animatedLights.erase(m_animatedLights.begin() + index);
    }
}

void Scene::addAnimatedPrimitive(const AnimatedPrimitive& ap) {
    m_animatedPrimitives.push_back(ap);
}

void Scene::removeAnimatedPrimitive(int index) {
    if (index >= 0 && index < static_cast<int>(m_animatedPrimitives.size())) {
        m_animatedPrimitives.erase(m_animatedPrimitives.begin() + index);
    }
}

void Scene::addTimeline(std::shared_ptr<Timeline> timeline) {
    m_timelines.push_back(timeline);
}

void Scene::removeTimeline(int index) {
    if (index >= 0 && index < static_cast<int>(m_timelines.size())) {
        m_timelines.erase(m_timelines.begin() + index);
    }
}

void Scene::addRelationship(const std::string& from, const std::string& rel, const std::string& to, float stiffness, float damping) {
    m_relationships.push_back({from, rel, to, stiffness, damping, {0.0f, 0.0f, 0.0f}});
}

float Scene::getTotalKineticEnergy() const {
    float total = 0.0f;
    for (const auto& p : m_primitives) {
        total += p->getKineticEnergy();
    }
    return total;
}

float Scene::getTotalPotentialEnergy() const {
    float total = 0.0f;
    for (const auto& p : m_primitives) {
        total += p->getPotentialEnergy();
    }
    for (const auto& rel : m_relationships) {
        if (rel.stiffness <= 0.0f) continue;
        std::shared_ptr<Primitive> fromPrim = nullptr, toPrim = nullptr;
        for (const auto& p : m_primitives) {
            if (p->getName() == rel.from) fromPrim = p;
            if (p->getName() == rel.to) toPrim = p;
        }
        if (fromPrim && toPrim) {
            Vec3 delta = toPrim->getPosition() - fromPrim->getPosition();
            total += 0.5f * rel.stiffness * delta.dot(delta);
        }
    }
    return total;
}

float Scene::getTotalSystemEnergy() const {
    return getTotalKineticEnergy() + getTotalPotentialEnergy();
}

void Scene::update(float deltaTime) {
    if (deltaTime <= 0.0f) return;

    m_time += deltaTime;

    for (auto& tl : m_timelines) {
        tl->update(deltaTime);
    }

    for (auto& al : m_animatedLights) {
        al.update(deltaTime, m_time);
    }

    for (auto& ap : m_animatedPrimitives) {
        ap.update(deltaTime, m_time);
    }

    for (auto& prim : m_primitives) {
        if (prim->getRotationSpeed().length() > 0.00001f) {
            prim->setRotation(prim->getRotation() + prim->getRotationSpeed() * deltaTime);
        }
        prim->updateWorldTransform();
    }

    auto cam = getActiveCamera();
    if (cam && cam->isOrbitEnabled()) {
        cam->updateOrbit(deltaTime);
    }

    if (m_physicsEnabled) {
        // Physical Mass-Spring-Damper Dynamics Simulation
        for (auto& prim : m_primitives) {
            if (prim->getSelfStiffness() > 0.0f || prim->getSelfDamping() > 0.0f) {
                Vec3 disp = prim->getPosition() - prim->getRestPosition();
                Vec3 fSelfSpring = disp * (-prim->getSelfStiffness());
                Vec3 fSelfDamping = prim->getVelocity() * (-prim->getSelfDamping());
                prim->addForce(fSelfSpring + fSelfDamping);
            }
        }

        for (auto& rel : m_relationships) {
            if (rel.stiffness <= 0.0f && rel.damping <= 0.0f) continue;
            std::shared_ptr<Primitive> fromPrim = nullptr, toPrim = nullptr;
            for (auto& p : m_primitives) {
                if (p->getName() == rel.from) fromPrim = p;
                if (p->getName() == rel.to) toPrim = p;
            }
            if (!fromPrim || !toPrim) continue;

            if (rel.rel == "supports") {
                float supporterTop = fromPrim->getPosition().y + fromPrim->getScale().y * 0.5f;
                float targetY = supporterTop + toPrim->getScale().y * 0.5f;
                float diffY = toPrim->getPosition().y - targetY;

                float fSpringY = -rel.stiffness * diffY;
                float relVelY = toPrim->getVelocity().y - fromPrim->getVelocity().y;
                float fDampingY = -rel.damping * relVelY;
                float totalFy = fSpringY + fDampingY;

                toPrim->addForce({0.0f, totalFy, 0.0f});
                fromPrim->addForce({0.0f, -totalFy, 0.0f});
            } else if (rel.rel == "mounted_on") {
                Vec3 currentOffset = fromPrim->getPosition() - toPrim->getPosition();
                Vec3 targetOffset = fromPrim->getRestPosition() - toPrim->getRestPosition();
                Vec3 disp = currentOffset - targetOffset;

                Vec3 fSpring = disp * (-rel.stiffness);
                Vec3 relVel = fromPrim->getVelocity() - toPrim->getVelocity();
                Vec3 fDamping = relVel * (-rel.damping);
                Vec3 totalF = fSpring + fDamping;

                fromPrim->addForce(totalF);
                toPrim->addForce(totalF * (-1.0f));
            }
        }

        for (auto& prim : m_primitives) {
            if (prim->getNetForce().length() > 0.00001f || prim->getVelocity().length() > 0.00001f) {
                Vec3 acc = prim->getNetForce() * (1.0f / prim->getMass());
                prim->setAcceleration(acc);

                Vec3 vel = prim->getVelocity() + acc * deltaTime;
                prim->setVelocity(vel);

                Vec3 pos = prim->getPosition() + vel * deltaTime;
                prim->setPosition(pos);
            }
            prim->clearForces(); // Clear accumulated forces at end of step
            prim->updateWorldTransform();
        }
    }

    // Kinematic multi-pass relationship propagation for zero-stiffness structural relationships
    for (int pass = 0; pass < 3; ++pass) {
        for (auto& rel : m_relationships) {
            if (rel.stiffness > 0.0f) continue; // Skip physical relationships handled by forces
            std::shared_ptr<Primitive> fromPrim = nullptr, toPrim = nullptr;
            for (auto& p : m_primitives) {
                if (p->getName() == rel.from) fromPrim = p;
                if (p->getName() == rel.to) toPrim = p;
            }
            if (!fromPrim || !toPrim) continue;
            if (rel.rel == "supports") {
                Vec3 supporterPos = fromPrim->getPosition();
                Vec3 supporterScale = fromPrim->getScale();
                Vec3 supportedPos = toPrim->getPosition();
                Vec3 supportedScale = toPrim->getScale();
                float supporterTop = supporterPos.y + supporterScale.y * 0.5f;
                float supportedY = supporterTop + supportedScale.y * 0.5f;
                toPrim->setPosition({supportedPos.x, supportedY, supportedPos.z});
                toPrim->updateWorldTransform();
            } else if (rel.rel == "mounted_on") {
                Vec3 mountPos = toPrim->getPosition();
                Vec3 fromPos = fromPrim->getPosition();
                float yOffset = fromPos.y - mountPos.y;
                fromPrim->setPosition({mountPos.x, mountPos.y + yOffset, mountPos.z});
                fromPrim->updateWorldTransform();
            }
        }
    }
}

} // namespace hse
