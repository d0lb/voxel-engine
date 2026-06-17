#pragma once
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

class Camera {
public:
    struct FrustumPlane {
        float a, b, c, d;
        void normalize();  
    };

    Camera(glm::vec3 position, glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = -90.0f, float pitch = 0.0f);
    glm::mat4 getViewMatrix() const;
    void processMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);
    void processKeyboardMovement(glm::vec3 direction, float deltaTime);
    void setProjection(const glm::mat4& proj) { m_Projection = proj; }
    void extractFrustumPlanes();
    bool isAABBVisible(const glm::vec3& min, const glm::vec3& max) const;

    glm::vec3 getFront() const { return m_Front; }
    glm::vec3 getUp() const { return m_Up; }
    glm::vec3 getPosition() const { return m_Position; }
    void setSpeed(float speed) { m_Speed = speed; }

private:
    glm::vec3 m_Position, m_Front, m_Up, m_Right, m_WorldUp;
    float m_Yaw, m_Pitch;
    float m_Speed, m_Sensitivity;
    glm::mat4 m_Projection;
    FrustumPlane m_Frustum[6];

    void updateCameraVectors();
};