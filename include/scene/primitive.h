#pragma once
#include "math/vec3.h"
#include "math/mat4.h"
#include "math/bounding_box.h"
#include <vector>
#include <cstdint>
#include <string>
#include <memory>
#include <algorithm>

namespace hse {

class Material;

enum class PrimitiveType { Triangle, Quad, Cube, Sphere, Cylinder, Group };

class Primitive {
public:
    Primitive(PrimitiveType type = PrimitiveType::Triangle);
    ~Primitive();

    Primitive(const Primitive&) = delete;
    Primitive& operator=(const Primitive&) = delete;
    Primitive(Primitive&&) = delete;
    Primitive& operator=(Primitive&&) = delete;

    void setPosition(const Vec3& position);
    void setRotation(const Vec3& rotation);
    void setScale(const Vec3& scale);
    void setColor(const Vec3& color);
    void setParent(Primitive* parent) {
        if (m_parent == parent) return;
        if (m_parent) {
            auto& pc = m_parent->m_children;
            pc.erase(std::remove(pc.begin(), pc.end(), this), pc.end());
        }
        m_parent = parent;
        if (m_parent) {
            m_parent->m_children.push_back(this);
        }
    }
    void setParent(std::shared_ptr<Primitive> parent) { setParent(parent.get()); }
    Primitive* getParent() const { return m_parent; }
    const std::vector<Primitive*>& getChildren() const { return m_children; }
    void addChild(Primitive* child) { if (child) child->setParent(this); }
    void addChild(std::shared_ptr<Primitive> child) { if (child) child->setParent(this); }
    void computeWorldMatrix() { updateWorldTransform(); }
    void setName(const std::string& name) { m_name = name; }
    const std::string& getName() const { return m_name; }

    PrimitiveType getType() const { return m_type; }
    uint64_t getID() const { return m_id; }
    void setID(uint64_t id) { m_id = id; }

    void setMaterial(std::shared_ptr<Material> mat);
    std::shared_ptr<Material> getMaterial() const { return m_material; }

    float getExplosionFactor() const { return m_explosionFactor; }
    void setExplosionFactor(float f) { m_explosionFactor = f; }

    const Vec3& getRotationSpeed() const { return m_rotationSpeed; }
    void setRotationSpeed(const Vec3& r) { m_rotationSpeed = r; }

    const Vec3& getPosition() const { return m_position; }
    const Vec3& getRotation() const { return m_rotation; }
    const Vec3& getScale() const { return m_scale; }
    const Vec3& getColor() const { return m_color; }
    const Vec3& getWorldPosition() const { return m_worldPosition; }
    const Vec3& getWorldRotation() const { return m_worldRotation; }
    const Vec3& getWorldScale() const { return m_worldScale; }
    const Mat4& getWorldTransform() const { return m_worldTransform; }
    const Mat4& getWorldMatrix() const { return m_worldTransform; }
    void updateWorldTransform();

    // Physical state & dynamics
    void setMass(float mass) { m_mass = (mass > 0.0001f ? mass : 0.0001f); }
    float getMass() const { return m_mass; }

    void setRestPosition(const Vec3& pos) { m_restPosition = pos; }
    const Vec3& getRestPosition() const { return m_restPosition; }

    void setVelocity(const Vec3& vel) { m_velocity = vel; }
    const Vec3& getVelocity() const { return m_velocity; }

    void setAcceleration(const Vec3& acc) { m_acceleration = acc; }
    const Vec3& getAcceleration() const { return m_acceleration; }

    void addForce(const Vec3& force) { m_netForce += force; }
    void clearForces() { m_netForce = {0.0f, 0.0f, 0.0f}; }
    const Vec3& getNetForce() const { return m_netForce; }

    void setSelfStiffness(float k) { m_selfStiffness = k; }
    float getSelfStiffness() const { return m_selfStiffness; }

    void setSelfDamping(float c) { m_selfDamping = c; }
    float getSelfDamping() const { return m_selfDamping; }

    float getKineticEnergy() const { return 0.5f * m_mass * m_velocity.dot(m_velocity); }
    float getPotentialEnergy() const {
        Vec3 disp = m_position - m_restPosition;
        return 0.5f * m_selfStiffness * disp.dot(disp);
    }
    float getTotalEnergy() const { return getKineticEnergy() + getPotentialEnergy(); }

    BoundingBox getBoundingBox() const {
        Vec3 half = m_worldScale * 0.5f;
        return BoundingBox(m_worldPosition - half, m_worldPosition + half);
    }
    BoundingBox getAggregateBoundingBox() const {
        BoundingBox bb = getBoundingBox();
        for (const auto* child : m_children) {
            if (child) {
                BoundingBox cbb = child->getAggregateBoundingBox();
                bb.min.x = std::min(bb.min.x, cbb.min.x);
                bb.min.y = std::min(bb.min.y, cbb.min.y);
                bb.min.z = std::min(bb.min.z, cbb.min.z);
                bb.max.x = std::max(bb.max.x, cbb.max.x);
                bb.max.y = std::max(bb.max.y, cbb.max.y);
                bb.max.z = std::max(bb.max.z, cbb.max.z);
            }
        }
        return bb;
    }

    const std::vector<float>& getVertices() const { return m_vertices; }
    const std::vector<uint32_t>& getIndices() const { return m_indices; }

    unsigned int getVAO() const { return m_vao; }
    unsigned int getVBO() const { return m_vbo; }
    unsigned int getEBO() const { return m_ebo; }
    size_t getIndexCount() const { return m_indices.size(); }

    void uploadGPU();
    void bind() const;
    void unbind() const;

private:
    PrimitiveType m_type;
    std::string m_name;
    uint64_t m_id = 0;
    std::shared_ptr<Material> m_material;
    float m_explosionFactor = 0.0f;
    Vec3 m_rotationSpeed{0.0f, 0.0f, 0.0f};

    Vec3 m_position{0.0f, 0.0f, 0.0f};
    Vec3 m_rotation{0.0f, 0.0f, 0.0f};
    Vec3 m_scale{1.0f, 1.0f, 1.0f};
    Vec3 m_color{1.0f, 0.5f, 0.2f};
    Primitive* m_parent = nullptr;
    std::vector<Primitive*> m_children;
    Vec3 m_worldPosition{0.0f, 0.0f, 0.0f};
    Vec3 m_worldRotation{0.0f, 0.0f, 0.0f};
    Vec3 m_worldScale{1.0f, 1.0f, 1.0f};
    Mat4 m_worldTransform;

    // Physical harmonic state
    float m_mass = 1.0f;
    Vec3 m_restPosition{0.0f, 0.0f, 0.0f};
    Vec3 m_velocity{0.0f, 0.0f, 0.0f};
    Vec3 m_acceleration{0.0f, 0.0f, 0.0f};
    Vec3 m_netForce{0.0f, 0.0f, 0.0f};
    float m_selfStiffness = 0.0f;
    float m_selfDamping = 0.0f;

    std::vector<float> m_vertices;
    std::vector<uint32_t> m_indices;

    unsigned int m_vao = 0;
    unsigned int m_vbo = 0;
    unsigned int m_ebo = 0;
    bool m_uploaded = false;

    void generateGeometry();
};

inline bool operator==(Primitive* raw, const std::shared_ptr<Primitive>& sp) { return raw == (sp ? sp.get() : nullptr); }
inline bool operator==(const std::shared_ptr<Primitive>& sp, Primitive* raw) { return (sp ? sp.get() : nullptr) == raw; }

} // namespace hse
