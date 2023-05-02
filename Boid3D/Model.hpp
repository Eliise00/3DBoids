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
    char const*               _filename; // will be "assets/models/penguin.obj"
    std::vector<unsigned int> _indices;
    GLuint                    _vbo;
    GLuint                    _vao;
    GLuint                    _ibo;

public:
    explicit Model(char const* filename)
        : _filename(filename), _vbo(0), _vao(0), _ibo(0){};

    GLuint getVbo() const
    {
        return _vbo;
    }

    GLuint getVao() const
    {
        return _vao;
    }

    GLuint getIbo() const
    {
        return _ibo;
    }

    std::vector<unsigned int>& getIndices()
    {
        return _indices;
    }

    void clearModel()
    {
        glDeleteBuffers(1, &_vbo);
        glDeleteVertexArrays(1, &_vao);
    }

    void initModel()
    {
        tinyobj::attrib_t                model_attrib;
        std::vector<tinyobj::shape_t>    model_shapes;
        std::vector<tinyobj::material_t> model_materials;

        std::string model_warn, model_err;

        bool ret = tinyobj::LoadObj(&model_attrib, &model_shapes, nullptr, &model_warn, &model_err, _filename);

        if (!model_warn.empty())
        {
            std::cout << "Warning: " << model_warn << std::endl;
        }
        if (!model_err.empty())
        {
            std::cerr << "Error: " << model_err << std::endl;
        }
        if (!ret)
        {
            exit(1);
        }

        std::vector<glimac::ShapeVertex> model_vertices;
        // TODO : récupérer le nombre de vertices
        // pas qu'il soit en dur (3561)

        for (int i = 0; i < 3561; i++)
        {
            glimac::ShapeVertex model_newVertex = glimac::ShapeVertex(

                // POSITION
                glm::vec3(
                    tinyobj::real_t(model_attrib.vertices[i * 3]),
                    tinyobj::real_t(model_attrib.vertices[i * 3 + 1]),
                    tinyobj::real_t(model_attrib.vertices[i * 3 + 2])
                ),

                // NORMAL
                glm::vec3(
                    tinyobj::real_t(model_attrib.normals[i * 3 + 0]), // nx
                    tinyobj::real_t(model_attrib.normals[i * 3 + 1]), // ny
                    tinyobj::real_t(model_attrib.normals[i * 3 + 2])  // nz
                ),

                // TEXTURE_COORDINATES
                glm::vec2(
                    tinyobj::real_t(model_attrib.texcoords[i * 2 + 0]), // tx
                    tinyobj::real_t(model_attrib.texcoords[i * 2 + 1])  // ty
                )
            );
            model_vertices.push_back(model_newVertex);
        }

        std::vector<unsigned int> _indices;
        for (const auto& shape : model_shapes)
        {
            for (const auto& index : shape.mesh.indices)
            {
                _indices.push_back(index.vertex_index);
            }
        }

        /////// GL code
        // Create a VBO for the model
        glGenBuffers(1, &_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, _vbo);
        glBufferData(GL_ARRAY_BUFFER, model_vertices.size() * sizeof(glimac::ShapeVertex), model_vertices.data(), GL_STATIC_DRAW);

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

        std::cout << "Model Initialised" << std::endl;
        std::cout << "size of model_vertices: " << model_vertices.size() << std::endl;
        std::cout << "size of indices: " << _indices.size() << std::endl;
    }
};

#endif