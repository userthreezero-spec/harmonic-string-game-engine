#include "scene/primitive.h"
#include <GL/glew.h>

namespace hse {

uint64_t Primitive::s_nextID = 1;
uint64_t Primitive::nextID() { return s_nextID++; }

Primitive::Primitive(PrimitiveType type) : m_id(s_nextID++), m_type(type) {
    generateGeometry();
}

Primitive::~Primitive() {
    if (m_uploaded) {
        glDeleteVertexArrays(1, &m_vao);
        glDeleteBuffers(1, &m_vbo);
        glDeleteBuffers(1, &m_ebo);
    }
}

void Primitive::setPosition(const Vec3& position) { m_position = position; }
void Primitive::setRotation(const Vec3& rotation) { m_rotation = rotation; }
void Primitive::setScale(const Vec3& scale) { m_scale = scale; }
void Primitive::setColor(const Vec3& color) { m_color = color; }
void Primitive::setRotationSpeed(const Vec3& speed) { m_rotationSpeed = speed; }
void Primitive::setTexture(std::shared_ptr<Texture> texture) { m_texture = texture; }

void Primitive::generateGeometry() {
    switch (m_type) {
        case PrimitiveType::Triangle:
            m_vertices = {
                -0.5f, -0.5f, 0.0f,  0.0f, 0.0f,
                 0.5f, -0.5f, 0.0f,  1.0f, 0.0f,
                 0.0f,  0.5f, 0.0f,  0.5f, 1.0f
            };
            m_indices = { 0, 1, 2 };
            break;

        case PrimitiveType::Quad:
            m_vertices = {
                -0.5f, -0.5f, 0.0f,  0.0f, 0.0f,
                 0.5f, -0.5f, 0.0f,  1.0f, 0.0f,
                 0.5f,  0.5f, 0.0f,  1.0f, 1.0f,
                -0.5f,  0.5f, 0.0f,  0.0f, 1.0f
            };
            m_indices = { 0, 1, 2, 2, 3, 0 };
            break;

        case PrimitiveType::Cube:
            m_vertices = {
                // Front
                -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
                 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
                 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
                -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
                // Back
                -0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
                 0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
                 0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
                -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
                // Top
                -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
                 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
                 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
                -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
                // Bottom
                -0.5f, -0.5f,  0.5f,  0.0f, 1.0f,
                 0.5f, -0.5f,  0.5f,  1.0f, 1.0f,
                 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
                -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
                // Right
                 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
                 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
                 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
                 0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
                // Left
                -0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
                -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
                -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
                -0.5f,  0.5f,  0.5f,  1.0f, 1.0f
            };
            m_indices = {
                0, 1, 2, 2, 3, 0,       // Front
                4, 5, 6, 6, 7, 4,       // Back
                8, 9, 10, 10, 11, 8,    // Top
                12, 13, 14, 14, 15, 12, // Bottom
                16, 17, 18, 18, 19, 16, // Right
                20, 21, 22, 22, 23, 20  // Left
            };
            break;

        case PrimitiveType::Sphere:
            m_vertices = { 0.0f, 0.0f, 0.0f };
            m_indices = { 0 };
            break;
    }
}

void Primitive::uploadGPU() {
    if (m_uploaded) return;

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);

    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(float), m_vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(uint32_t), m_indices.data(), GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Texture attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
    m_uploaded = true;
}

void Primitive::bind() const {
    glBindVertexArray(m_vao);
}

void Primitive::unbind() const {
    glBindVertexArray(0);
}

} // namespace hse
