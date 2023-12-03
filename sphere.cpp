#include "Sphere.h"

void Sphere::updateParams(int param1, int param2) {
    m_vertexData = std::vector<float>();
    m_param1 = (param1 < 2) ? 2 : param1;
    m_param2 = (param2 < 3) ? 3 : param2;
    setVertexData();
}

void Sphere::makeTile(glm::vec3 topLeft,
                      glm::vec3 topRight,
                      glm::vec3 bottomLeft,
                      glm::vec3 bottomRight) {
    // Calculate the normals based on the direction from origin to the vertex
    glm::vec3 normal_topLeft = glm::normalize(topLeft);
    glm::vec3 normal_topRight = glm::normalize(topRight);
    glm::vec3 normal_bottomLeft = glm::normalize(bottomLeft);
    glm::vec3 normal_bottomRight = glm::normalize(bottomRight);

    // bottom-left triangle
    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, normal_topLeft);
    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, normal_bottomLeft);
    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, normal_bottomRight);
    // top-right triangle
    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, normal_topRight);
    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, normal_topLeft);
    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, normal_bottomRight);

}

void Sphere::makeWedge(float currentTheta, float nextTheta) {
    float r = 0.5f;
    float phiStep = glm::radians(180.f / m_param1);

    for(int i = 0; i < m_param1; ++i) {
        float currentPhi = i * phiStep;
        float nextPhi = (i + 1) * phiStep;

        glm::vec3 topLeft(r * glm::sin(currentPhi) * glm::sin(currentTheta),
                          r * glm::cos(currentPhi),
                          r * glm::sin(currentPhi) * glm::cos(currentTheta));

        glm::vec3 topRight(r * glm::sin(currentPhi) * glm::sin(nextTheta),
                           r * glm::cos(currentPhi),
                           r * glm::sin(currentPhi) * glm::cos(nextTheta));

        glm::vec3 bottomLeft(r * glm::sin(nextPhi) * glm::sin(currentTheta),
                             r * glm::cos(nextPhi),
                             r * glm::sin(nextPhi) * glm::cos(currentTheta));

        glm::vec3 bottomRight(r * glm::sin(nextPhi) * glm::sin(nextTheta),
                              r * glm::cos(nextPhi),
                              r * glm::sin(nextPhi) * glm::cos(nextTheta));

        makeTile(topLeft, topRight, bottomLeft, bottomRight);
    }
}

void Sphere::makeSphere() {
    float thetaStep = glm::radians(360.f / m_param2);
    for(int i = 0; i < m_param2; ++i) {
        float currentTheta = i * thetaStep;
        float nextTheta = (i + 1) * thetaStep;
        makeWedge(currentTheta, nextTheta);
    }
}

void Sphere::setVertexData() {
    makeSphere();
}

void Sphere::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}
