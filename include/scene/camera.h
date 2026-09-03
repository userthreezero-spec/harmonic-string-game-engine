#pragma once
#include "math/vec3.h"
#include "math/mat4.h"
#include <string>

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
    void setOrbitRadius(float radius);
    float getOrbitRadius() const { return m_orbitRadius; }
    void setOrbitSpeed(float speed);
    float getOrbitSpeed() const { return m_orbitSpeed; }
    void setOrbitCenter(const Vec3& center);
    void enableOrbit(bool enabled);
    bool isOrbitEnabled() const;
    void updateOrbit(float deltaTime);

    void offsetOrbit(float yawDeg, float pitchDeg) {
        m_orbitYaw += yawDeg * 3.14159265f / 180.0f;
        m_orbitPitch += pitchDeg * 3.14159265f / 180.0f;
        updateOrbit(0.0f);
    }
    void pan(float dx, float dy) {
        m_position = m_position + Vec3{dx, dy, 0.0f};
        m_target = m_target + Vec3{dx, dy, 0.0f};
    }
    void zoom(float delta) {
        m_orbitRadius -= delta;
        if (m_orbitRadius < 0.1f) m_orbitRadius = 0.1f;
        updateOrbit(0.0f);
    }

    const Vec3& getPosition() const { return m_position; }
    const Vec3& getTarget() const { return m_target; }
    float getFOV() const { return m_fovDeg; }
    float getNearPlane() const { return m_nearPlane; }
    float getFarPlane() const { return m_farPlane; }
    float getAspectRatio() const { return m_aspectRatio; }
    ProjectionType getProjectionType() const { return m_type; }
    const Mat4& getViewMatrix() const { return m_viewMatrix; }
    const Mat4& getProjectionMatrix() const { return m_projectionMatrix; }

    enum class CameraMode { FreeFly, Orbit, Editor };

    void setMode(CameraMode mode) { m_mode = mode; }
    CameraMode getMode() const { return m_mode; }

    void orbitRotate(float dx, float dy);
    void panView(float dx, float dy);

    float getMouseSensitivity() const { return m_mouseSensitivity; }
    void setMouseSensitivity(float sensitivity) { m_mouseSensitivity = sensitivity; }

    float getMoveSpeed() const { return m_moveSpeed; }
    void setMoveSpeed(float speed) { m_moveSpeed = speed; }
    float getYaw() const { return m_yawDeg; }
    float getPitch() const { return m_pitchDeg; }

    void processKeyboard(const std::string& direction, float deltaTime, bool boost = false);
    void processMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);
    void processMouseScroll(float yoffset);
    void resetFocus(const Vec3& position = {0.0f, 4.0f, 15.0f}, const Vec3& target = {0.0f, 2.5f, 0.0f});

    void update();

private:
    ProjectionType m_type;
    CameraMode m_mode = CameraMode::FreeFly;
    Vec3 m_position{0.0f, 0.0f, 5.0f};
    Vec3 m_target{0.0f, 0.0f, 0.0f};
    Vec3 m_up{0.0f, 1.0f, 0.0f};
    float m_fovDeg = 45.0f;
    float m_nearPlane = 0.1f;
    float m_farPlane = 1000.0f;
    float m_aspectRatio = 16.0f / 9.0f;

    Mat4 m_viewMatrix;
    Mat4 m_projectionMatrix;

    bool m_orbitEnabled = false;
    float m_orbitRadius = 5.0f;
    float m_orbitYaw = 0.0f;
    float m_orbitPitch = 0.0f;
    float m_orbitSpeed = 1.0f;
    Vec3 m_orbitCenter{0.0f, 0.0f, 0.0f};

    float m_yawDeg = -90.0f;
    float m_pitchDeg = 0.0f;
    float m_moveSpeed = 6.0f;
    float m_mouseSensitivity = 0.12f;
};

} // namespace hse
