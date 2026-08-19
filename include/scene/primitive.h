#pragma once
#include "math/vec3.h"
#include "math/mat4.h"
#include "math/bounding_box.h"
#include "renderer/material.h"
#include <vector>
#include <cstdint>
#include <string>
#include <memory>

namespace hse {

enum class PrimitiveType { Triangle, Quad, Cube, Sphere, Group };

class Primitive : public std::enable_shared_from_this<Primitive> {
public:
    Primitive(PrimitiveType type = PrimitiveType::Triangle);
    ~Primitive();

    Primitive(const Primitive&) = delete;
    Primitive& operator=(const Primitive&) = delete;
    Primitive(Primitive&&) = delete;
    Primitive& operator=(Primitive&&) = delete;

    uint64_t getID() const { return m_id; }
    const std::string& getName() const { return m_name; }
    void setName(const std::string& name) { m_name = name; }

    void setPosition(const Vec3& position);
    void setRotation(const Vec3& rotation);
    void setScale(const Vec3& scale);
    void setRotationSpeed(const Vec3& speed);

    // Material management
    void setMaterial(std::shared_ptr<Material> material) { m_material = material; }
    std::shared_ptr<Material> getMaterial() const { return m_material; }

    // Transform hierarchy
    void setParent(std::shared_ptr<Primitive> parent);
    std::shared_ptr<Primitive> getParent() const;
    const std::vector<std::shared_ptr<Primitive>>& getChildren() const { return m_children; }
    const Mat4& getWorldMatrix() const { return m_worldMatrix; }
    void computeWorldMatrix();
    void addChild(std::shared_ptr<Primitive> child);
    void removeChild(uint64_t childID);

    PrimitiveType getType() const { return m_type; }

    const Vec3& getPosition() const { return m_position; }
    const Vec3& getRotation() const { return m_rotation; }
    const Vec3& getScale() const { return m_scale; }
    const Vec3& getRotationSpeed() const { return m_rotationSpeed; }

    const std::vector<float>& getVertices() const { return m_vertices; }
    const std::vector<uint32_t>& getIndices() const { return m_indices; }

    unsigned int getVAO() const { return m_vao; }
    unsigned int getVBO() const { return m_vbo; }
    unsigned int getEBO() const { return m_ebo; }
    size_t getIndexCount() const { return m_indices.size(); }

    BoundingBox getBoundingBox() const;
    BoundingBox getAggregateBoundingBox() const;

    void setExplosionFactor(float f) { m_explosionFactor = f; }
    float getExplosionFactor() const { return m_explosionFactor; }

    void uploadGPU();
    void bind() const;
    void unbind() const;

    static uint64_t nextID();

private:
    static uint64_t s_nextID;
    uint64_t m_id;
    std::string m_name;
    PrimitiveType m_type;
    Vec3 m_position{0.0f, 0.0f, 0.0f};
    Vec3 m_rotation{0.0f, 0.0f, 0.0f};
    Vec3 m_scale{1.0f, 1.0f, 1.0f};
    Vec3 m_rotationSpeed{0.0f, 0.0f, 0.0f};
    float m_explosionFactor = 0.0f;

    std::shared_ptr<Material> m_material = nullptr;

    // Transform hierarchy
    std::weak_ptr<Primitive> m_parent;
    std::vector<std::shared_ptr<Primitive>> m_children;
    Mat4 m_worldMatrix;

    std::vector<float> m_vertices;
    std::vector<uint32_t> m_indices;

    unsigned int m_vao = 0;
    unsigned int m_vbo = 0;
    unsigned int m_ebo = 0;
    bool m_uploaded = false;

    void generateGeometry();
};

} // namespace hse
