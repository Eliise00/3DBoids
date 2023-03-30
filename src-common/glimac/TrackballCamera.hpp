#ifndef OPENGL_TPS_TEMPLATE_TRACKBALLCAMERA_H
#define OPENGL_TPS_TEMPLATE_TRACKBALLCAMERA_H
#include <glm/glm.hpp>
#include "glm/ext/matrix_transform.hpp"

class TrackballCamera{

public:
    float m_Distance;
    float m_AngleX;
    float m_AngleY;

    void moveFront(float delta){
        m_Distance += delta;
    }
    void rotateLeft(float degrees){
        m_AngleX += degrees;
    }
    void rotateUp(float degrees){
        m_AngleY += degrees;
    }

    glm::mat4 getViewMatrix() const{
        // Convertir les angles en radians
        float angleX_rad = glm::radians(m_AngleX);
        float angleY_rad = glm::radians(m_AngleY);

        // Calculer les coordonnées de la position de la caméra
        float camPosX = m_Distance * sin(angleX_rad) * cos(angleY_rad);
        float camPosY = m_Distance * sin(angleY_rad);
        float camPosZ = m_Distance * cos(angleX_rad) * cos(angleY_rad);

        // Construire la matrice de vue
        glm::vec3 cameraPosition(camPosX, camPosY, camPosZ);
        glm::mat4 viewMatrix(1.0f);
        viewMatrix = glm::rotate(viewMatrix, angleX_rad, glm::vec3(0.0f, 1.0f, 0.0f));
        viewMatrix = glm::rotate(viewMatrix, angleY_rad, glm::vec3(cos(angleX_rad), 0.0f, sin(angleX_rad)));
        viewMatrix = glm::translate(viewMatrix, -cameraPosition);

        return viewMatrix;
    }

};


#endif // OPENGL_TPS_TEMPLATE_TRACKBALLCAMERA_H
