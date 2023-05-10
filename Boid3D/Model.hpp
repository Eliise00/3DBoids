#ifndef _MODEL_HPP
#define _MODEL_HPP

#include <cstdlib>
#include <glimac/common.hpp>
#include <iostream>
#include <vector>
#include "glm/gtc/type_ptr.hpp"
#include "tiny_obj_loader.h"

class Model {
private:
    tinyobj::attrib_t                _attrib;
    std::vector<tinyobj::shape_t>    _shapes;
    std::vector<tinyobj::material_t> _materials;
    std::vector<glimac::ShapeVertex> _vertices;
    std::vector<unsigned int>        _indices;
    GLuint                           _vbo;
    GLuint                           _ibo;
    GLuint                           _vao;

public:
    Model(char const* filename)
        : _vbo(0), _ibo(0), _vao(0)
    {
        std::string warn;
        std::string err;

        bool ret = tinyobj::LoadObj(&_attrib, &_shapes, nullptr, &warn, &err, filename);

        if (!warn.empty())
        {
            std::cout << "Warning: " << warn << std::endl;
        }
        if (!err.empty())
        {
            std::cerr << "Error: " << err << std::endl;
        }
        if (!ret)
        {
            exit(1);
        }
    };

    void setVertices()
    {
        for (int i = 0; i < static_cast<int>(_attrib.GetVertices().size() / 3); i++)
        {
            glimac::ShapeVertex newVertex = glimac::ShapeVertex(

                // POSITION
                glm::vec3(
                    tinyobj::real_t(_attrib.vertices[i * 3]),
                    tinyobj::real_t(_attrib.vertices[i * 3 + 1]),
                    tinyobj::real_t(_attrib.vertices[i * 3 + 2])
                ),

                // NORMAL
                glm::vec3(
                    tinyobj::real_t(_attrib.normals[i * 3 + 0]), // nx
                    tinyobj::real_t(_attrib.normals[i * 3 + 1]), // ny
                    tinyobj::real_t(_attrib.normals[i * 3 + 2])  // nz
                ),

                // TEXTURE_COORDINATES
                glm::vec2(
                    tinyobj::real_t(_attrib.texcoords[i * 2 + 0]), // tx
                    tinyobj::real_t(_attrib.texcoords[i * 2 + 1])  // ty
                )
            );
            _vertices.push_back(newVertex);
        }
    }

    void setIndices()
    {
        for (const auto& shape : _shapes)
        {
            for (const auto& index : shape.mesh.indices)
            {
                _indices.push_back(index.vertex_index);
            }
        }
    }

    void glCode()
    {
        glGenBuffers(1, &_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(glimac::ShapeVertex), _vertices.data(), GL_STATIC_DRAW);

        // Create an IBO for the model
        glGenBuffers(1, &_ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(unsigned int), _indices.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        // Create a VAO for the model
        glGenVertexArrays(1, &_vao);
        glBindVertexArray(_vao);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ibo);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);

        ////// the order is important
        const GLuint VERTEX_ATTR_POSITION  = 0;
        const GLuint VERTEX_ATTR_NORMAL    = 1;
        const GLuint VERTEX_ATTR_TEXCOORDS = 2;

        glEnableVertexAttribArray(VERTEX_ATTR_POSITION);
        glEnableVertexAttribArray(VERTEX_ATTR_NORMAL);
        glEnableVertexAttribArray(VERTEX_ATTR_TEXCOORDS);

        glVertexAttribPointer(VERTEX_ATTR_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(glimac::ShapeVertex), (const GLvoid*)offsetof(glimac::ShapeVertex, position));
        glVertexAttribPointer(VERTEX_ATTR_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(glimac::ShapeVertex), (const GLvoid*)offsetof(glimac::ShapeVertex, normal));
        glVertexAttribPointer(VERTEX_ATTR_TEXCOORDS, 2, GL_FLOAT, GL_FALSE, sizeof(glimac::ShapeVertex), (const GLvoid*)offsetof(glimac::ShapeVertex, texCoords));

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void bindVertexArray() const
    {
        glBindVertexArray(_vao);
    }

    void debindVertexArray() const
    {
        glBindVertexArray(0);
    }

    void clearBuffers() const
    {
        glDeleteBuffers(1, &_vbo);
        glDeleteVertexArrays(1, &_vao);
    }

    std::vector<unsigned int> getIndices()
    {
        return _indices;
    }
};

#endif