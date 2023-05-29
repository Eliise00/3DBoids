#ifndef OPENGL_TPS_TEMPLATE_THIRDPERSONCAMERA_H
#define OPENGL_TPS_TEMPLATE_THIRDPERSONCAMERA_H
#include <glm/glm.hpp>
#include "glm/ext/matrix_transform.hpp"
#include "glm/fwd.hpp"

class ThirdPersonCamera {
public:
    glm::vec3 m_targetPos;
    float     m_Distance;
    float     m_AngleX;
    float     m_AngleY;

    ThirdPersonCamera(glm::vec3 targetPos)
        : m_targetPos(targetPos), m_Distance(2.0f), m_AngleX(0.0f), m_AngleY(0.0f)
    {
    }

    void moveFront(float delta)
    {
        m_Distance += delta;
    }
    void rotateLeft(float degrees)
    {
        m_AngleX += degrees;
    }
    void rotateUp(float degrees)
    {
        m_AngleY += degrees;
    }

    void update(glm::vec3 targetPos)
    {
        m_targetPos = targetPos;
        targetPos.z -= 0.2f;
    }

    glm::mat4 getViewMatrix() const
    {
        // Convert angles to radians
        float angleX_rad = glm::radians(m_AngleX);
        float angleY_rad = glm::radians(m_AngleY);

        // Calculate camera position coordinates
        float camPosX = m_Distance * sin(angleX_rad) * cos(angleY_rad);
        float camPosY = m_Distance * sin(angleY_rad);
        float camPosZ = m_Distance * cos(angleX_rad) * cos(angleY_rad);

        // Build the view matrix
        glm::vec3 cameraPosition(camPosX, camPosY, camPosZ);
        glm::mat4 viewMatrix(1.0f);
        viewMatrix = glm::rotate(viewMatrix, angleX_rad, glm::vec3(0.0f, 1.0f, 0.0f));
        viewMatrix = glm::rotate(viewMatrix, angleY_rad, glm::vec3(cos(angleX_rad), 0.0f, sin(angleX_rad)));
        viewMatrix = glm::translate(viewMatrix, -cameraPosition);
        viewMatrix = glm::translate(viewMatrix, -m_targetPos);

        return viewMatrix;
    }

};

#endif // OPENGL_TPS_TEMPLATE_THIRDPERSONCAMERA_H
