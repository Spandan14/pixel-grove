#ifndef EMISSIVE_S_H
#define EMISSIVE_S_H


#include "scenedata.h"
#include "src/utils/drawables.h"
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>

class Emissive_S : public Drawables
{
public:
    Emissive_S(glm::vec4 location, std::vector<SceneLightData>& lights);
    std::vector<GLfloat> generateShape()  { return this->m_vertexData ; };
    void freeMesh() override ;
    void drawMesh() override;
    int getSize() override {return m_vertexData.size()/6;}
private:
    void bindMesh();
    void insertVec3(std::vector<float> &data, glm::vec3 v);
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

    SceneLightData s_light;
};

#endif // EMISSIVE_S_H
