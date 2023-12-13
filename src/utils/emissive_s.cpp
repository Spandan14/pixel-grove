#include "emissive_s.h"
#include <iostream>

Emissive_S::Emissive_S(glm::vec4 location, std::vector<SceneLightData>& lights){
    this->setVertexData();
    glGenBuffers(1, &m_vbo);
    glGenVertexArrays(1, &m_vao);
    this->s_light.type = LightType::LIGHT_POINT;
    this->s_light.pos = location;
    this->s_light.function = glm::vec3(0.5, 0.5, 0.5);
    lights.push_back(s_light);
    std::cout<<m_vertexData.size()<<std::endl;
    this->bindMesh();
}

void Emissive_S::freeMesh(){
    glDeleteBuffers(1, &m_vbo);
    glDeleteVertexArrays(1, &m_vao);
}

void Emissive_S::drawMesh(){
    glBindVertexArray(this->m_vao);
    glDrawArrays(GL_TRIANGLES, 0, this->m_vertexData.size() / 6);
    glBindVertexArray(0);
}

void Emissive_S::bindMesh(){
    glBindBuffer(GL_ARRAY_BUFFER,m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * this->m_vertexData.size(), this->m_vertexData.data(), GL_STATIC_DRAW);
    glBindVertexArray(m_vao);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat),0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), reinterpret_cast<void*>(3*sizeof(GLfloat)));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Emissive_S::makeTile(glm::vec3 topLeft,
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

void Emissive_S::makeWedge(float currentTheta, float nextTheta) {
    float r = 0.1f;
    float phiStep = M_PI/10.f;

    for(int i = 0; i < 10; ++i) {
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

void Emissive_S::makeSphere() {
    float thetaStep = 2 * M_PI/10.f;
    for(int i = 0; i < 10; ++i) {
        float currentTheta = i * thetaStep;
        float nextTheta = (i + 1) * thetaStep;
        makeWedge(currentTheta, nextTheta);
    }
}

void Emissive_S::setVertexData() {
    makeSphere();
}

void Emissive_S::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}
