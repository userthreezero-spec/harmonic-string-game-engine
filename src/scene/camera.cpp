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
