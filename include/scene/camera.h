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
    const Vec3& getTarget() const { return m_target; }
    float getFOV() const { return m_fovDeg; }
    float getNearPlane() const { return m_nearPlane; }
    float getFarPlane() const { return m_farPlane; }
    float getAspectRatio() const { return m_aspectRatio; }
    const Mat4& getViewMatrix() const { return m_viewMatrix; }
    const Mat4& getProjectionMatrix() const { return m_projectionMatrix; }

    void setOrbitRadius(float radius);
    void setOrbitSpeed(float speed);
    void enableOrbit(bool enabled);
    bool isOrbitEnabled() const { return m_orbitEnabled; }
    float getOrbitRadius() const { return m_orbitRadius; }
    float getOrbitSpeed() const { return m_orbitSpeed; }
    void updateOrbit(float deltaTime);

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

    float m_orbitRadius = 5.0f;
    float m_orbitYaw = 0.0f;
    float m_orbitSpeed = 1.0f;
    bool  m_orbitEnabled = false;
};

} // namespace hse
