#include "scene/primitive.h"
#include <GL/glew.h>

namespace hse {

static uint64_t s_nextID = 1;

Primitive::Primitive(PrimitiveType type) : m_type(type), m_id(s_nextID++) {
    generateGeometry();
}

Primitive::~Primitive() {
    if (m_uploaded) {
        glDeleteVertexArrays(1, &m_vao);
        glDeleteBuffers(1, &m_vbo);
        glDeleteBuffers(1, &m_ebo);
    }
}

void Primitive::setPosition(const Vec3& position) {
    m_position = position;
}
void Primitive::setRotation(const Vec3& rotation) { m_rotation = rotation; }
void Primitive::setScale(const Vec3& scale) { m_scale = scale; }
void Primitive::setColor(const Vec3& color) { m_color = color; }

void Primitive::updateWorldTransform() {
    float parentExplosion = 0.0f;
    Primitive* p = m_parent;
    while (p) {
        parentExplosion += p->getExplosionFactor();
        p = p->getParent();
    }
    Vec3 effectivePos = m_position * (1.0f + parentExplosion);

    if (m_parent) {
        const Mat4& parentWorld = m_parent->getWorldTransform();
        Mat4 local = Mat4::translate(effectivePos)
                   * Mat4::rotate(m_rotation.x, {1, 0, 0})
                   * Mat4::rotate(m_rotation.y, {0, 1, 0})
                   * Mat4::rotate(m_rotation.z, {0, 0, 1})
                   * Mat4::scale(m_scale);
        m_worldTransform = parentWorld * local;
        m_worldPosition = parentWorld * effectivePos;
        m_worldRotation = m_parent->getWorldRotation() + m_rotation;
        m_worldScale = {m_parent->getWorldScale().x * m_scale.x,
                        m_parent->getWorldScale().y * m_scale.y,
                        m_parent->getWorldScale().z * m_scale.z};
    } else {
        m_worldTransform = Mat4::translate(m_position)
                         * Mat4::rotate(m_rotation.x, {1, 0, 0})
                         * Mat4::rotate(m_rotation.y, {0, 1, 0})
                         * Mat4::rotate(m_rotation.z, {0, 0, 1})
                         * Mat4::scale(m_scale);
        m_worldPosition = m_position;
        m_worldRotation = m_rotation;
        m_worldScale = m_scale;
    }
    for (auto* child : m_children) {
        if (child) child->updateWorldTransform();
    }
}

void Primitive::generateGeometry() {
    m_vertices.clear();
    m_indices.clear();

    switch (m_type) {
        case PrimitiveType::Triangle: {
            Vec3 v0(-0.5f, -0.5f, 0.0f);
            Vec3 v1( 0.5f, -0.5f, 0.0f);
            Vec3 v2( 0.0f,  0.5f, 0.0f);
            Vec3 edge1 = v1 - v0;
            Vec3 edge2 = v2 - v0;
            Vec3 n = edge1.cross(edge2).normalized();
            m_vertices = {
                v0.x, v0.y, v0.z, 0.0f, 0.0f, n.x, n.y, n.z,
                v1.x, v1.y, v1.z, 1.0f, 0.0f, n.x, n.y, n.z,
                v2.x, v2.y, v2.z, 0.5f, 1.0f, n.x, n.y, n.z
            };
            m_indices = { 0, 1, 2 };
            break;
        }

        case PrimitiveType::Quad: {
            Vec3 v0(-0.5f, -0.5f, 0.0f);
            Vec3 v1( 0.5f, -0.5f, 0.0f);
            Vec3 v2( 0.5f,  0.5f, 0.0f);
            Vec3 v3(-0.5f,  0.5f, 0.0f);
            Vec3 edge1 = v1 - v0;
            Vec3 edge2 = v2 - v0;
            Vec3 n = edge1.cross(edge2).normalized();
            m_vertices = {
                v0.x, v0.y, v0.z, 0.0f, 0.0f, n.x, n.y, n.z,
                v1.x, v1.y, v1.z, 1.0f, 0.0f, n.x, n.y, n.z,
                v2.x, v2.y, v2.z, 1.0f, 1.0f, n.x, n.y, n.z,
                v3.x, v3.y, v3.z, 0.0f, 1.0f, n.x, n.y, n.z
            };
            m_indices = { 0, 1, 2, 2, 3, 0 };
            break;
        }

        case PrimitiveType::Cube: {
            struct Face { Vec3 verts[4]; Vec3 normal; };
            Face faces[6] = {
                {{ Vec3(-0.5f,-0.5f, 0.5f), Vec3( 0.5f,-0.5f, 0.5f), Vec3( 0.5f, 0.5f, 0.5f), Vec3(-0.5f, 0.5f, 0.5f) }, Vec3( 0, 0, 1)},
                {{ Vec3( 0.5f,-0.5f,-0.5f), Vec3(-0.5f,-0.5f,-0.5f), Vec3(-0.5f, 0.5f,-0.5f), Vec3( 0.5f, 0.5f,-0.5f) }, Vec3( 0, 0,-1)},
                {{ Vec3(-0.5f, 0.5f, 0.5f), Vec3( 0.5f, 0.5f, 0.5f), Vec3( 0.5f, 0.5f,-0.5f), Vec3(-0.5f, 0.5f,-0.5f) }, Vec3( 0, 1, 0)},
                {{ Vec3(-0.5f,-0.5f,-0.5f), Vec3( 0.5f,-0.5f,-0.5f), Vec3( 0.5f,-0.5f, 0.5f), Vec3(-0.5f,-0.5f, 0.5f) }, Vec3( 0,-1, 0)},
                {{ Vec3( 0.5f,-0.5f, 0.5f), Vec3( 0.5f,-0.5f,-0.5f), Vec3( 0.5f, 0.5f,-0.5f), Vec3( 0.5f, 0.5f, 0.5f) }, Vec3( 1, 0, 0)},
                {{ Vec3(-0.5f,-0.5f,-0.5f), Vec3(-0.5f,-0.5f, 0.5f), Vec3(-0.5f, 0.5f, 0.5f), Vec3(-0.5f, 0.5f,-0.5f) }, Vec3(-1, 0, 0)}
            };
            float uvs[4][2] = {{0.0f,0.0f}, {1.0f,0.0f}, {1.0f,1.0f}, {0.0f,1.0f}};
            for (int f = 0; f < 6; f++) {
                uint32_t base = f * 4;
                for (int v = 0; v < 4; v++) {
                    m_vertices.insert(m_vertices.end(), {
                        faces[f].verts[v].x, faces[f].verts[v].y, faces[f].verts[v].z,
                        uvs[v][0], uvs[v][1],
                        faces[f].normal.x, faces[f].normal.y, faces[f].normal.z
                    });
                }
                m_indices.insert(m_indices.end(), {
                    base, base+1, base+2, base+2, base+3, base
                });
            }
            break;
        }

        case PrimitiveType::Sphere:
            m_vertices = { 0.0f, 0.0f, 0.0f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f };
            m_indices = { 0 };
            break;

        case PrimitiveType::Cylinder:
        case PrimitiveType::Group:
        default:
            // Cylinder/Group fallback geometry
            m_vertices = {
                -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
                 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
                 0.0f,  0.5f, 0.0f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f
            };
            m_indices = { 0, 1, 2 };
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

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);

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
