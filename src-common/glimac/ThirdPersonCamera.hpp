//
// Created by elise on 06/04/2023.
//

#ifndef OPENGL_TPS_TEMPLATE_THIRDPERSONCAMERA_HPP
#define OPENGL_TPS_TEMPLATE_THIRDPERSONCAMERA_HPP
#include <glm/glm.hpp>
#include "glm/ext/matrix_transform.hpp"
#include "glm/fwd.hpp"

class ThirdPersonCamera {
public:
    glm::vec3 m_targetPos;
    float     m_distanceFromTarget;
    glm::vec3 m_Position;
    float     m_Phi;
    float     m_Theta;
    glm::vec3 m_FrontVector;
    glm::vec3 m_LeftVector;
    glm::vec3 m_UpVector;

    ThirdPersonCamera(glm::vec3 targetPos, float distanceFromTarget)
    {
        m_targetPos          = targetPos;
        m_distanceFromTarget = distanceFromTarget;
        m_Position           = glm::vec3(0.0f, 0.0f, 0.0f);
        m_Phi                = glm::pi<float>(); // yaw
        m_Theta              = 0.0f;             // pitch

        computeDirectionVectors();
    }

    void moveLeft(float t)
    {
        m_Position += t * m_LeftVector;
    }

    void moveFront(float t)
    {
        m_Position += t * m_FrontVector;
    }

    void rotateLeft(float degrees)
    {
        float radians = glm::radians(degrees);
        m_Phi += radians;
    }

    void rotateUp(float degrees)
    {
        float radians = glm::radians(degrees);
        m_Theta += radians;
    }

    void update(glm::vec3 targetPos)
    {
        m_targetPos      = targetPos;
        float h_distance = calculateHorizontalDistance();
        float v_distance = calculateVerticalDistance();
        calculateCameraPosition(h_distance, v_distance);
        computeDirectionVectors();
    }

    glm::mat4 getViewMatrix() const
    {
        return glm::lookAt(m_Position, m_Position + m_FrontVector, m_UpVector);
    }

    glm::vec3 getPosition() const
    {
        return m_Position;
    }

private:
    void computeDirectionVectors()
    {
        m_FrontVector = glm::vec3(glm::cos(m_Theta) * glm::sin(m_Phi), glm::sin(m_Theta), glm::cos(m_Theta) * glm::cos(m_Phi));
        m_LeftVector  = glm::vec3(glm::sin(m_Phi + glm::pi<float>() / 2.0f), 0., glm::cos(m_Phi + glm::pi<float>() / 2.0f));
        m_UpVector    = glm::cross(m_LeftVector, m_FrontVector);
        ;
    }

    float calculateHorizontalDistance()
    {
        return m_distanceFromTarget * glm::cos(m_Theta);
    }

    float calculateVerticalDistance()
    {
        return m_distanceFromTarget * glm::sin(m_Theta);
    }

    void calculateCameraPosition(float h_distance, float v_distance)
    {
        float offsetX = h_distance * glm::sin(m_Phi);
        float offsetZ = h_distance * glm::cos(m_Phi);

        m_Position.x = m_targetPos.x - offsetX;
        m_Position.z = m_targetPos.x - offsetZ;
        m_Position.y = m_targetPos.y + v_distance;
    }
};

#endif // OPENGL_TPS_TEMPLATE_THIRDPERSONCAMERA_HPP
