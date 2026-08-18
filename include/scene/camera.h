#pragma once
#include "math/vec3.h"
#include "math/mat4.h"

namespace hse {

enum class ProjectionType { Perspective, Orthographic };

class Camera {
public:
    Camera(ProjectionType type = ProjectionType::Perspective);

    void setPosition(const Vec3& position);
    void lookAt(const Vec3& target);
    void setFOV(float fovDeg);
    void setNearPlane(float near);
    void setFarPlane(float far);
    void setAspectRatio(float ratio);

    ProjectionType getProjectionType() const { return m_type; }
    const Vec3& getPosition() const { return m_position; }
    const Mat4& getViewMatrix() const { return m_viewMatrix; }
    const Mat4& getProjectionMatrix() const { return m_projectionMatrix; }

    void update();

private:
    ProjectionType m_type;
    Vec3 m_position{0.0f, 0.0f, 5.0f};
    Vec3 m_target{0.0f, 0.0f, 0.0f};
    Vec3 m_up{0.0f, 1.0f, 0.0f};
    float m_fovDeg = 45.0f;
    float m_nearPlane = 0.1f;
    float m_farPlane = 1000.0f;
    float m_aspectRatio = 16.0f / 9.0f;

    Mat4 m_viewMatrix;
    Mat4 m_projectionMatrix;
};

} // namespace hse
