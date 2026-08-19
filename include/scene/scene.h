#pragma once
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include <cstdint>

namespace hse {

class Primitive;
class Camera;
class Material;
class Light;

class Scene {
public:
    Scene(const std::string& name = "Untitled");
    ~Scene();

    const std::string& getName() const { return m_name; }
    size_t getPrimitiveCount() const { return m_primitives.size(); }
    size_t getCameraCount() const { return m_cameras.size(); }
    size_t getLightCount() const { return m_lights.size(); }

    void addPrimitive(std::shared_ptr<Primitive> primitive);
    void removePrimitive(int index);
    bool removePrimitiveByID(uint64_t id);
    void update(float deltaTime);

    std::shared_ptr<Primitive> findByID(uint64_t id) const;
    std::shared_ptr<Primitive> findByName(const std::string& name) const;
    int findIndexByID(uint64_t id) const;

    const std::vector<std::shared_ptr<Primitive>>& getPrimitives() const {
        return m_primitives;
    }

    void addCamera(std::shared_ptr<Camera> camera);
    void setActiveCamera(int index);
    std::shared_ptr<Camera> getActiveCamera() const;

    // Light management
    void addLight(std::shared_ptr<Light> light);
    void removeLight(int index);
    const std::vector<std::shared_ptr<Light>>& getLights() const { return m_lights; }

    // Material management
    void addMaterial(std::shared_ptr<Material> material);
    std::shared_ptr<Material> getMaterial(const std::string& name) const;
    const std::unordered_map<std::string, std::shared_ptr<Material>>& getMaterials() const { return m_materials; }

    // Transform hierarchy
    bool reparent(uint64_t childID, uint64_t parentID);
    std::vector<std::shared_ptr<Primitive>> getRoots() const;
    void computeAllWorldMatrices();

private:
    std::string m_name;
    std::vector<std::shared_ptr<Primitive>> m_primitives;
    std::unordered_map<uint64_t, size_t> m_idIndex;
    std::vector<std::shared_ptr<Camera>> m_cameras;
    int m_activeCameraIndex = 0;
    std::vector<std::shared_ptr<Light>> m_lights;
    std::unordered_map<std::string, std::shared_ptr<Material>> m_materials;

    void rebuildIDIndex();
};

} // namespace hse
