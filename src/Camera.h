#pragma once
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

class Camera {
public:
    Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f),
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
        float yaw = -90.0f, float pitch = 0.0f);

    glm::mat4 getViewMatrix() const;
    void processMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);
    void processKeyboardMovement(glm::vec3 direction, float deltaTime, float speed = 2.5f);

    // Getters
    glm::vec3 getPosition() const { return m_Position; }
    glm::vec3 getFront() const { return m_Front; }
    glm::vec3 getUp() const { return m_Up; }
    float getYaw() const { return m_Yaw; }
    float getPitch() const { return m_Pitch; }

    // Setters (для изменения скорости)
    void setSpeed(float speed) { m_Speed = speed; }
    void setSensitivity(float sensitivity) { m_Sensitivity = sensitivity; }

private:
    void updateCameraVectors();

    glm::vec3 m_Position;
    glm::vec3 m_Front;
    glm::vec3 m_Up;
    glm::vec3 m_Right;
    glm::vec3 m_WorldUp;

    float m_Yaw;
    float m_Pitch;
    float m_Speed;
    float m_Sensitivity;
};