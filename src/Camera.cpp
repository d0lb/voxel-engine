#include "Camera.h"
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <cmath>

// реализация normalize через полное имя
void Camera::FrustumPlane::normalize() {
    float mag = std::sqrt(a * a + b * b + c * c);
    a /= mag; b /= mag; c /= mag; d /= mag;
}

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
    : m_Position(position), m_WorldUp(up), m_Yaw(yaw), m_Pitch(pitch),
    m_Speed(5.5f), m_Sensitivity(0.1f), m_Projection(1.0f) {
    updateCameraVectors();
}

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(m_Position, m_Position + m_Front, m_Up);
}

void Camera::processMouseMovement(float xoffset, float yoffset, bool constrainPitch) {
    xoffset *= m_Sensitivity;
    yoffset *= m_Sensitivity;
    m_Yaw += xoffset;
    m_Pitch += yoffset;
    if (constrainPitch) {
        if (m_Pitch > 89.0f) m_Pitch = 89.0f;
        if (m_Pitch < -89.0f) m_Pitch = -89.0f;
    }
    updateCameraVectors();
}

void Camera::processKeyboardMovement(glm::vec3 direction, float deltaTime) {
    float velocity = m_Speed * deltaTime;
    m_Position += direction * velocity;
}

void Camera::updateCameraVectors() {
    glm::vec3 front;
    front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
    front.y = sin(glm::radians(m_Pitch));
    front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
    m_Front = glm::normalize(front);
    m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));
    m_Up = glm::normalize(glm::cross(m_Right, m_Front));
}

void Camera::extractFrustumPlanes() {
    glm::mat4 VP = m_Projection * getViewMatrix();
    glm::vec4 row0(VP[0][0], VP[1][0], VP[2][0], VP[3][0]);
    glm::vec4 row1(VP[0][1], VP[1][1], VP[2][1], VP[3][1]);
    glm::vec4 row2(VP[0][2], VP[1][2], VP[2][2], VP[3][2]);
    glm::vec4 row3(VP[0][3], VP[1][3], VP[2][3], VP[3][3]);

    m_Frustum[0] = { row3.x + row0.x, row3.y + row0.y, row3.z + row0.z, row3.w + row0.w }; 
    m_Frustum[1] = { row3.x - row0.x, row3.y - row0.y, row3.z - row0.z, row3.w - row0.w }; 
    m_Frustum[2] = { row3.x + row1.x, row3.y + row1.y, row3.z + row1.z, row3.w + row1.w }; 
    m_Frustum[3] = { row3.x - row1.x, row3.y - row1.y, row3.z - row1.z, row3.w - row1.w };
    m_Frustum[4] = { row3.x + row2.x, row3.y + row2.y, row3.z + row2.z, row3.w + row2.w }; 
    m_Frustum[5] = { row3.x - row2.x, row3.y - row2.y, row3.z - row2.z, row3.w - row2.w };

    for (int i = 0; i < 6; ++i)
        m_Frustum[i].normalize();
}

bool Camera::isAABBVisible(const glm::vec3& min, const glm::vec3& max) const {
    for (int i = 0; i < 6; ++i) {
        const FrustumPlane& p = m_Frustum[i];
        glm::vec3 positive(
            p.a > 0 ? max.x : min.x,
            p.b > 0 ? max.y : min.y,
            p.c > 0 ? max.z : min.z
        );
        if (p.a * positive.x + p.b * positive.y + p.c * positive.z + p.d < 0)
            return false;
    }
    return true;
}