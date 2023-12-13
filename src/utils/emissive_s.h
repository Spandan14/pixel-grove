#ifndef EMISSIVE_S_H
#define EMISSIVE_S_H


#include "src/utils/drawables.h"
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>

class Emissive_S : public Drawables
{
public:
    Emissive_S(glm::vec3& location);

    std::vector<GLfloat> generateShape()  { return this->m_vertexData ; }
    void freeMesh() override;
    void drawMesh() override;
    int getSize() override;
private:
    void insertVec3(std::vector<GLfloat> &data, glm::vec3 v);
    void setVertexData();
    void makeTile(glm::vec3 topLeft,
                  glm::vec3 topRight,
                  glm::vec3 bottomLeft,
                  glm::vec3 bottomRight);
    void makeWedge(float currTheta, float nextTheta);
    void makeSphere();

    std::vector<float> m_vertexData;
    float m_radius = 0.01;

    GLuint m_vbo;

    GLuint m_vao;

};

#endif // EMISSIVE_S_H
