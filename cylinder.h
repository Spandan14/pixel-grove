#pragma once

#include <vector>
#include <glm/glm.hpp>

class Cylinder
{
public:
    void updateParams(int param1, int param2);
    std::vector<float> generateShape() { return m_vertexData; }

private:
    void insertVec3(std::vector<float> &data, glm::vec3 v);
    void setVertexData();

    std::vector<float> m_vertexData;
    int m_param1;
    int m_param2;
    float m_radius = 0.5;
    void makeCap(bool top);
    void makeSideWalls();
    void makeSurface();
    void insertRectangle(glm::vec3 topLeft, glm::vec3 topRight, glm::vec3 bottomLeft, glm::vec3 bottomRight);
    void insertTriangle(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3);
    void makeSideWalls2();
    void addTriangle(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3);
    void createCap(bool isTop, float radius, float yPos);
    void makeTopCap();
    void makeBottomCap();
    void makeTile(glm::vec3 topLeft,
                  glm::vec3 topRight,
                  glm::vec3 bottomLeft,
                  glm::vec3 bottomRight, bool top);
};
