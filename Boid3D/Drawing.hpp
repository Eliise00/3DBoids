#ifndef _DRAWING_HPP
#define _DRAWING_HPP

#include "Boid3D.hpp"
#include "Program.hpp"
#include "glimac/common.hpp"
#include "glm/gtc/type_ptr.hpp"

void drawPenguin(int i, const PenguinProgram& penguinProgram, const std::vector<unsigned int>& indices, glm::mat4 ProjMatrix, glm::mat4 MVMatrix, glm::mat4 NormalMatrix, std::vector<glm::vec3> Ka, std::vector<glm::vec3> Kd, std::vector<glm::vec3> Ks, std::vector<float> Shininess)
{
    // Set uniform variables
    glUniformMatrix4fv(penguinProgram.uMVPMatrix, 1, GL_FALSE, glm::value_ptr(ProjMatrix * MVMatrix));
    glUniformMatrix4fv(penguinProgram.uMVMatrix, 1, GL_FALSE, glm::value_ptr(MVMatrix));
    glUniformMatrix4fv(penguinProgram.uNormalMatrix, 1, GL_FALSE, glm::value_ptr(NormalMatrix));

    glUniform3fv(penguinProgram.uKa, 1, glm::value_ptr(Ka[i]));
    glUniform3fv(penguinProgram.uKd, 1, glm::value_ptr(Kd[i]));
    glUniform3fv(penguinProgram.uKs, 1, glm::value_ptr(Ks[i]));
    glUniform1f(penguinProgram.uShininess, Shininess[i]);
    glUniform3fv(penguinProgram.uLightPos_vs, 1, glm::value_ptr(glm::vec3(-1, -1, -1)));
    glUniform3fv(penguinProgram.uLightIntensity, 1, glm::value_ptr(glm::vec3(1, 1, 1)));

    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);
}

void drawModel(const ModelProgram& modelProgram, const std::vector<unsigned int>& indices, glm::mat4 ProjMatrix, glm::mat4 MVMatrix, glm::mat4 NormalMatrix)
{
    // Set uniform variables
    glUniformMatrix4fv(modelProgram.uMVPMatrix, 1, GL_FALSE, glm::value_ptr(ProjMatrix * MVMatrix));
    glUniformMatrix4fv(modelProgram.uMVMatrix, 1, GL_FALSE, glm::value_ptr(MVMatrix));
    glUniformMatrix4fv(modelProgram.uNormalMatrix, 1, GL_FALSE, glm::value_ptr(NormalMatrix));

    glUniform3fv(modelProgram.uKa, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 1.0f)));
    glUniform3fv(modelProgram.uKd, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 1.0f)));
    glUniform3fv(modelProgram.uKs, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 1.0f)));

    glUniform3fv(modelProgram.uLightPos_vs, 1, glm::value_ptr(glm::vec3(-1, -1, -1)));
    glUniform3fv(modelProgram.uLightIntensity, 1, glm::value_ptr(glm::vec3(1, 1, 1)));

    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);
}
#endif // _DRAWING_HPP
