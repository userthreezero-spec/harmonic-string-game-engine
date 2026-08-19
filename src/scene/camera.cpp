#include "scene/camera.h"
#include <cmath>

namespace hse {

Camera::Camera(ProjectionType type) : m_type(type) {
    update();
}

void Camera::setPosition(const Vec3& position) {
    m_position = position;
    update();
}

void Camera::lookAt(const Vec3& target) {
    m_target = target;
    update();
}

void Camera::setFOV(float fovDeg) {
    m_fovDeg = fovDeg;
    update();
}

void Camera::setNearPlane(float near) {
    m_nearPlane = near;
    update();
}

void Camera::setFarPlane(float far) {
    m_farPlane = far;
    update();
}

void Camera::setAspectRatio(float ratio) {
    m_aspectRatio = ratio;
    update();
}

void Camera::setOrbitRadius(float radius) {
    m_orbitRadius = radius;
}

void Camera::setOrbitSpeed(float speed) {
    m_orbitSpeed = speed;
}

void Camera::enableOrbit(bool enabled) {
    m_orbitEnabled = enabled;
    if (enabled) {
        updateOrbit(0.0f);
    }
}

void Camera::updateOrbit(float deltaTime) {
    if (m_orbitEnabled && deltaTime > 0.0f) {
        m_orbitYaw += m_orbitSpeed * deltaTime;
    }

    if (m_orbitEnabled || deltaTime <= 0.0f) {
        float cosPitch = std::cos(m_orbitPitch);
        m_position.x = m_target.x + m_orbitRadius * cosPitch * std::cos(m_orbitYaw);
        m_position.y = m_target.y + m_orbitRadius * std::sin(m_orbitPitch);
        m_position.z = m_target.z + m_orbitRadius * cosPitch * std::sin(m_orbitYaw);

        update();
    }
}

void Camera::offsetOrbit(float yawDeg, float pitchDeg) {
    const float degToRad = 3.14159265f / 180.0f;
    m_orbitYaw += yawDeg * degToRad;
    m_orbitPitch += pitchDeg * degToRad;

    float limit = 89.0f * degToRad;
    if (m_orbitPitch > limit) m_orbitPitch = limit;
    if (m_orbitPitch < -limit) m_orbitPitch = -limit;

    updateOrbit(0.0f);
}

void Camera::pan(float x, float y) {
    Vec3 forward = (m_target - m_position).normalized();
    Vec3 right = forward.cross(m_up).normalized();
    Vec3 upLocal = right.cross(forward).normalized();

    Vec3 delta = (right * x) + (upLocal * y);
    m_target += delta;
    m_position += delta;

    update();
}

void Camera::zoom(float delta) {
    m_orbitRadius -= delta;
    if (m_orbitRadius < 0.1f) m_orbitRadius = 0.1f;
    updateOrbit(0.0f);
}

void Camera::update() {
    m_viewMatrix = Mat4::lookAt(m_position, m_target, m_up);

    if (m_type == ProjectionType::Perspective) {
        m_projectionMatrix = Mat4::perspective(m_fovDeg, m_aspectRatio, m_nearPlane, m_farPlane);
    } else {
        float halfW = m_aspectRatio * 5.0f;
        float halfH = 5.0f;
        m_projectionMatrix = Mat4::ortho(-halfW, halfW, -halfH, halfH, m_nearPlane, m_farPlane);
    }
}

} // namespace hse
