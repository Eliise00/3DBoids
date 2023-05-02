#ifndef _PROGRAM_HPP
#define _PROGRAM_HPP

#include <cstdlib>
#include <iostream>
#include <random>
#include "p6/p6.h"

struct PenguinProgram {
    p6::Shader m_Program;

    GLint uMVPMatrix;
    GLint uMVMatrix;
    GLint uNormalMatrix;
    GLint uTexture;
    GLint uKa;
    GLint uKd;
    GLint uKs;
    GLint uShininess;
    GLint uLightPos_vs;
    GLint uLightIntensity;

    PenguinProgram()
        : m_Program(p6::load_shader("shaders/3D.vs.glsl", "shaders/directionalLight.fs.glsl"))
    {
        uMVPMatrix    = glGetUniformLocation(m_Program.id(), "uMVPMatrix");
        uMVMatrix     = glGetUniformLocation(m_Program.id(), "uMVMatrix");
        uNormalMatrix = glGetUniformLocation(m_Program.id(), "uNormalMatrix");

        uTexture = glGetUniformLocation(m_Program.id(), "uTexture");

        uKa             = glGetUniformLocation(m_Program.id(), "uKa");
        uKd             = glGetUniformLocation(m_Program.id(), "uKd");
        uKs             = glGetUniformLocation(m_Program.id(), "uKs");
        uShininess      = glGetUniformLocation(m_Program.id(), "uShininess");
        uLightPos_vs    = glGetUniformLocation(m_Program.id(), "uLightPos_vs");
        uLightIntensity = glGetUniformLocation(m_Program.id(), "uLightIntensity");
    }
};

#endif