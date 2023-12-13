#ifndef MESH_H
#define MESH_H


#include "src/utils/drawables.h"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>


class Mesh : public Drawables
{
public:
    Mesh(const char* filepath);

    GLuint getVAO();

    void freeMesh() override;
    void drawMesh() override;
    int getSize() override {return m_mesh.size()/8;}


private:
    void bindMesh();
    bool loadMesh();
    const char* filepath;
    std::vector<float> m_mesh;
    GLuint m_vbo;
    GLuint m_vao;

};

#endif // MESH_H
