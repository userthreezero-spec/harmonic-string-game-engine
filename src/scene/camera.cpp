#include "scene/camera.h"
#include <cmath>
#include <string>

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
    Vec3 dir = (m_target - m_position).normalized();
    if (dir.length() > 0.0001f) {
        float clampY = dir.y > 1.0f ? 1.0f : (dir.y < -1.0f ? -1.0f : dir.y);
        m_pitchDeg = std::asin(clampY) * 180.0f / 3.1415926535f;
        m_yawDeg = std::atan2(dir.z, dir.x) * 180.0f / 3.1415926535f;
    }
    if (m_orbitEnabled) {
        Vec3 orbitDir = m_position - m_target;
        m_orbitRadius = orbitDir.length();
        m_orbitYaw = std::atan2(orbitDir.z, orbitDir.x);
        float r = (m_orbitRadius > 0.0001f ? m_orbitRadius : 1.0f);
        float clampRatio = orbitDir.y / r;
        if (clampRatio > 1.0f) clampRatio = 1.0f;
        if (clampRatio < -1.0f) clampRatio = -1.0f;
        m_orbitPitch = std::asin(clampRatio);
    }
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
    m_orbitYaw = 0.0f;
    m_orbitPitch = 0.0f;
    updateOrbit(0.0f);
}
void Camera::setOrbitSpeed(float speed) { m_orbitSpeed = speed; }
void Camera::setOrbitCenter(const Vec3& center) { m_orbitCenter = center; }
void Camera::enableOrbit(bool enabled) { m_orbitEnabled = enabled; }
bool Camera::isOrbitEnabled() const { return m_orbitEnabled; }

void Camera::updateOrbit(float deltaTime) {
    if (!m_orbitEnabled && deltaTime != 0.0f) return;
    if (deltaTime != 0.0f) {
        m_orbitYaw += m_orbitSpeed * deltaTime;
    }
    m_position.x = m_orbitCenter.x + m_orbitRadius * std::cos(m_orbitYaw) * std::cos(m_orbitPitch);
    m_position.y = m_orbitCenter.y + m_orbitRadius * std::sin(m_orbitPitch);
    m_position.z = m_orbitCenter.z + m_orbitRadius * std::sin(m_orbitYaw) * std::cos(m_orbitPitch);
    update();
}

void Camera::processKeyboard(const std::string& direction, float deltaTime, bool boost) {
    float speed = m_moveSpeed * (boost ? 2.5f : 1.0f) * deltaTime;
    Vec3 forward = (m_target - m_position).normalized();
    Vec3 right = forward.cross(m_up).normalized();

    if (direction == "FORWARD") {
        m_position += forward * speed;
        m_target += forward * speed;
    } else if (direction == "BACKWARD") {
        m_position -= forward * speed;
        m_target -= forward * speed;
    } else if (direction == "LEFT") {
        m_position -= right * speed;
        m_target -= right * speed;
    } else if (direction == "RIGHT") {
        m_position += right * speed;
        m_target += right * speed;
    } else if (direction == "UP") {
        m_position += m_up * speed;
        m_target += m_up * speed;
    } else if (direction == "DOWN") {
        m_position -= m_up * speed;
        m_target -= m_up * speed;
    }
    update();
}

void Camera::processMouseMovement(float xoffset, float yoffset, bool constrainPitch) {
    xoffset *= m_mouseSensitivity;
    yoffset *= m_mouseSensitivity;

    m_yawDeg += xoffset;
    m_pitchDeg += yoffset;

    if (constrainPitch) {
        if (m_pitchDeg > 89.0f) m_pitchDeg = 89.0f;
        if (m_pitchDeg < -89.0f) m_pitchDeg = -89.0f;
    }

    float yawRad = m_yawDeg * 3.14159265f / 180.0f;
    float pitchRad = m_pitchDeg * 3.14159265f / 180.0f;

    Vec3 front;
    front.x = std::cos(pitchRad) * std::cos(yawRad);
    front.y = std::sin(pitchRad);
    front.z = std::cos(pitchRad) * std::sin(yawRad);
    m_target = m_position + front.normalized();

    update();
}

void Camera::orbitRotate(float dx, float dy) {
    Vec3 dir = m_position - m_target;
    float radius = dir.length();
    if (radius < 0.01f) radius = 0.01f;

    float clampR = dir.y / radius;
    if (clampR > 1.0f) clampR = 1.0f;
    if (clampR < -1.0f) clampR = -1.0f;

    float currentYaw = std::atan2(dir.z, dir.x);
    float currentPitch = std::asin(clampR);

    float newYaw = currentYaw - dx * m_mouseSensitivity * 3.1415926535f / 180.0f;
    float newPitch = currentPitch + dy * m_mouseSensitivity * 3.1415926535f / 180.0f;

    float maxPitch = 88.0f * 3.1415926535f / 180.0f;
    if (newPitch > maxPitch) newPitch = maxPitch;
    if (newPitch < -maxPitch) newPitch = -maxPitch;

    m_position.x = m_target.x + radius * std::cos(newPitch) * std::cos(newYaw);
    m_position.y = m_target.y + radius * std::sin(newPitch);
    m_position.z = m_target.z + radius * std::cos(newPitch) * std::sin(newYaw);

    Vec3 lookDir = (m_target - m_position).normalized();
    float clampY = lookDir.y > 1.0f ? 1.0f : (lookDir.y < -1.0f ? -1.0f : lookDir.y);
    m_pitchDeg = std::asin(clampY) * 180.0f / 3.1415926535f;
    m_yawDeg = std::atan2(lookDir.z, lookDir.x) * 180.0f / 3.1415926535f;

    update();
}

void Camera::panView(float dx, float dy) {
    Vec3 forward = (m_target - m_position).normalized();
    Vec3 right = forward.cross(m_up).normalized();
    Vec3 up = right.cross(forward).normalized();

    float dist = (m_position - m_target).length();
    float factor = 0.002f * (dist < 1.0f ? 1.0f : dist);

    Vec3 delta = (right * (-dx) + up * dy) * factor * (m_mouseSensitivity * 10.0f);
    m_position += delta;
    m_target += delta;

    update();
}

void Camera::processMouseScroll(float yoffset) {
    m_moveSpeed += yoffset * 0.5f;
    if (m_moveSpeed < 0.5f) m_moveSpeed = 0.5f;
    if (m_moveSpeed > 50.0f) m_moveSpeed = 50.0f;
}

void Camera::resetFocus(const Vec3& position, const Vec3& target) {
    m_position = position;
    m_target = target;
    m_yawDeg = -90.0f;
    m_pitchDeg = 0.0f;
    m_moveSpeed = 6.0f;
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
