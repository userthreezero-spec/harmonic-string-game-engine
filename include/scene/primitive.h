#pragma once
#include "math/vec3.h"
#include "math/mat4.h"
#include "renderer/texture.h"
#include <vector>
#include <cstdint>
#include <string>
#include <memory>

namespace hse {

enum class PrimitiveType { Triangle, Quad, Cube, Sphere };

class Primitive {
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
    void setColor(const Vec3& color);
    void setRotationSpeed(const Vec3& speed);
    void setTexture(std::shared_ptr<Texture> texture);

    PrimitiveType getType() const { return m_type; }

    const Vec3& getPosition() const { return m_position; }
    const Vec3& getRotation() const { return m_rotation; }
    const Vec3& getScale() const { return m_scale; }
    const Vec3& getColor() const { return m_color; }
    const Vec3& getRotationSpeed() const { return m_rotationSpeed; }
    std::shared_ptr<Texture> getTexture() const { return m_texture; }
    bool hasTexture() const { return m_texture != nullptr; }

    const std::vector<float>& getVertices() const { return m_vertices; }
    const std::vector<uint32_t>& getIndices() const { return m_indices; }

    unsigned int getVAO() const { return m_vao; }
    unsigned int getVBO() const { return m_vbo; }
    unsigned int getEBO() const { return m_ebo; }
    size_t getIndexCount() const { return m_indices.size(); }

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
    Vec3 m_color{1.0f, 0.5f, 0.2f};
    Vec3 m_rotationSpeed{0.0f, 0.0f, 0.0f};

    std::vector<float> m_vertices;
    std::vector<uint32_t> m_indices;

    unsigned int m_vao = 0;
    unsigned int m_vbo = 0;
    unsigned int m_ebo = 0;
    bool m_uploaded = false;
    std::shared_ptr<Texture> m_texture = nullptr;

    void generateGeometry();
};

} // namespace hse
