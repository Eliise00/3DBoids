#pragma once

#include <glm/glm.hpp>

namespace glimac {

struct ShapeVertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
    ShapeVertex(glm::vec3 pos, glm::vec3 norm, glm::vec2 tex)
        :position(pos), normal(norm), texCoords(tex)
    {}
    ShapeVertex(){}
};

} // namespace glimac
