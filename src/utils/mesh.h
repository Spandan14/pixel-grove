#ifndef MESH_H
#define MESH_H


#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>
#include <vector>


class Mesh
{
public:
    Mesh(const char* filepath);

    GLuint getVAO();

    void freeMesh();
    void drawMesh();


private:
    void bindMesh();
    bool loadMesh();
    const char* filepath;
    std::vector<float> m_mesh;
    GLuint m_vbo;
    GLuint m_vao;

};

#endif // MESH_H
