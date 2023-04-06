//
// Created by elise on 06/04/2023.
//

#ifndef OPENGL_TPS_TEMPLATE_FREEFLYCAMERA_HPP
#define OPENGL_TPS_TEMPLATE_FREEFLYCAMERA_HPP
#include <glm/glm.hpp>
#include "glm/ext/matrix_transform.hpp"

class FreeflyCamera{

public:
    glm::vec3 m_Position;
    float m_Phi;
    float m_Theta;
    glm::vec3 m_FrontVector;
    glm::vec3 m_LeftVector;
    glm::vec3 m_UpVector;

    FreeflyCamera(){
        m_Position = glm::vec3(0.0f, 0.0f, 0.0f);
        m_Phi = glm::pi<float>();
        m_Theta = 0.0f;

        computeDirectionVectors();
    }

    void moveLeft(float t){
        m_Position += t * m_LeftVector;
    }

    void moveFront(float t){
        m_Position += t * m_FrontVector;
    }

    void rotateLeft(float degrees){
        float radians = glm::radians(degrees);
        m_Phi += radians;
        computeDirectionVectors();
    }

    void rotateUp(float degrees){
        float radians = glm::radians(degrees);
        m_Theta += radians;
        computeDirectionVectors();
    }

    glm::mat4 getViewMatrix() const{
        return glm::lookAt(m_Position, m_Position + m_FrontVector, m_UpVector);
    }

private :
    void computeDirectionVectors(){
        m_FrontVector = glm::vec3(glm::cos(m_Theta)*glm::sin(m_Phi), glm::sin(m_Theta), glm::cos(m_Theta)*glm::cos(m_Phi));
        m_LeftVector = glm::vec3(glm::sin(m_Phi + glm::pi<float>()/2.0f), 0., glm::cos(m_Phi + glm::pi<float>()/2.0f));
        m_UpVector = glm::cross(m_LeftVector, m_FrontVector);;
    }



};




#endif // OPENGL_TPS_TEMPLATE_FREEFLYCAMERA_HPP
