#include "scene/primitive.h"
#include <GL/glew.h>

namespace hse {

Primitive::Primitive(PrimitiveType type) : m_type(type) {
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

void Primitive::generateGeometry() {
    switch (m_type) {
        case PrimitiveType::Triangle:
            m_vertices = {
                -0.5f, -0.5f, 0.0f,
                 0.5f, -0.5f, 0.0f,
                 0.0f,  0.5f, 0.0f
            };
            m_indices = { 0, 1, 2 };
            break;

        case PrimitiveType::Quad:
            m_vertices = {
                -0.5f, -0.5f, 0.0f,
                 0.5f, -0.5f, 0.0f,
                 0.5f,  0.5f, 0.0f,
                -0.5f,  0.5f, 0.0f
            };
            m_indices = { 0, 1, 2, 2, 3, 0 };
            break;

        case PrimitiveType::Cube:
            m_vertices = {
                -0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f,
                 0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f,
                -0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f,
                 0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f
            };
            m_indices = {
                0,1,2, 2,3,0, 4,5,6, 6,7,4,
                0,4,7, 7,3,0, 1,5,6, 6,2,1,
                0,1,5, 5,4,0, 3,2,6, 6,7,3
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

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

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
