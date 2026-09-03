#pragma once
#include "math/vec3.h"
#include "renderer/material.h"
#include "scene/primitive.h"
#include "scene/light.h"
#include <vector>
#include <cstdint>
#include <memory>
#include <string>

namespace hse {

class Material;
class Primitive;
class Camera;
class Timeline;
struct AnimatedLight;
struct AnimatedPrimitive;

class Scene {
public:
    Scene(const std::string& name = "Untitled");
    ~Scene();

    const std::string& getName() const { return m_name; }

    void addPrimitive(std::shared_ptr<Primitive> primitive);
    void removePrimitive(int index);
    void removePrimitiveByID(uint64_t id);
    size_t getLightCount() const { return m_lights.size(); }

    const std::vector<std::shared_ptr<Primitive>>& getPrimitives() const {
        return m_primitives;
    }
    size_t getPrimitiveCount() const { return m_primitives.size(); }
    std::shared_ptr<Primitive> findByID(uint64_t id) const {
        for (const auto& p : m_primitives) {
            if (p->getID() == id) return p;
        }
        return nullptr;
    }

    void addCamera(std::shared_ptr<Camera> camera);
    void setActiveCamera(int index);
    std::shared_ptr<Camera> getActiveCamera() const;
    size_t getCameraCount() const { return m_cameras.size(); }

    void addMaterial(std::shared_ptr<Material> m) { m_materials.push_back(m); }
    const std::vector<std::shared_ptr<Material>>& getMaterials() const { return m_materials; }
    std::shared_ptr<Material> getMaterial(const std::string& name) const {
        for (const auto& m : m_materials) {
            if (m->getName() == name) return m;
        }
        return nullptr;
    }
    std::shared_ptr<Primitive> findByName(const std::string& name) const {
        for (const auto& p : m_primitives) {
            if (p->getName() == name) return p;
        }
        return nullptr;
    }

    std::vector<std::shared_ptr<Primitive>> getRoots() const {
        std::vector<std::shared_ptr<Primitive>> roots;
        for (const auto& p : m_primitives) {
            if (p->getParent() == nullptr) roots.push_back(p);
        }
        return roots;
    }

    bool reparent(uint64_t childID, uint64_t parentID) {
        auto child = findByID(childID);
        auto parent = findByID(parentID);
        if (child && parent) {
            child->setParent(parent);
            return true;
        }
        return false;
    }

    void computeAllWorldMatrices();

    void addLight(const Light& light);
    void addLight(std::shared_ptr<Light> light) { if (light) m_lights.push_back(*light); }
    void removeLight(int index);
    const std::vector<Light>& getLights() const { return m_lights; }
    Light& getLight(int index);

    void addAnimatedLight(const AnimatedLight& al);
    void removeAnimatedLight(int index);
    const std::vector<AnimatedLight>& getAnimatedLights() const { return m_animatedLights; }

    void addAnimatedPrimitive(const AnimatedPrimitive& ap);
    void removeAnimatedPrimitive(int index);
    const std::vector<AnimatedPrimitive>& getAnimatedPrimitives() const { return m_animatedPrimitives; }

    void addTimeline(std::shared_ptr<Timeline> timeline);
    void removeTimeline(int index);
    const std::vector<std::shared_ptr<Timeline>>& getTimelines() const { return m_timelines; }

    struct Relationship {
        std::string from;
        std::string rel;
        std::string to;
        float stiffness = 0.0f;
        float damping = 0.0f;
        Vec3 restOffset{0.0f, 0.0f, 0.0f};
    };
    void addRelationship(const std::string& from, const std::string& rel, const std::string& to, float stiffness = 0.0f, float damping = 0.0f);
    const std::vector<Relationship>& getRelationships() const { return m_relationships; }
    void clearRelationships() { m_relationships.clear(); }

    std::vector<uint64_t> getCollisions(uint64_t id);
    bool checkCollision(uint64_t id1, uint64_t id2);

    void enablePhysics(bool enabled) { m_physicsEnabled = enabled; }
    bool isPhysicsEnabled() const { return m_physicsEnabled; }
    float getTotalKineticEnergy() const;
    float getTotalPotentialEnergy() const;
    float getTotalSystemEnergy() const;

    void update(float deltaTime);
    float getTime() const { return m_time; }

private:
    std::string m_name;
    std::vector<std::shared_ptr<Primitive>> m_primitives;
    std::vector<std::shared_ptr<Camera>> m_cameras;
    int m_activeCameraIndex = 0;
    std::vector<Light> m_lights;
    std::vector<std::shared_ptr<Material>> m_materials;
    float m_time = 0.0f;
    std::vector<AnimatedLight> m_animatedLights;
    std::vector<AnimatedPrimitive> m_animatedPrimitives;
    std::vector<std::shared_ptr<Timeline>> m_timelines;
    std::vector<Relationship> m_relationships;
    bool m_physicsEnabled = true;
};

} // namespace hse
