#include "cylinder.h"

void Cylinder::updateParams(int param1, int param2) {
    m_vertexData = std::vector<float>();
    m_param1 = param1;
    m_param2 = (param2 < 3) ? 3 : param2;
    setVertexData();
}

void Cylinder::makeTopCap() {
    float r = 0.5f;
    float thetaStep = glm::radians(360.f / m_param2);
    for(int i = 0; i < m_param2; ++i) {
        float currentTheta = i * thetaStep;
        float nextTheta = (i + 1) * thetaStep;
        for(int j = 0; j < m_param1; ++j) {
            float step = 1.f / m_param1;

            glm::vec3 topLeft(r * j * step * glm::sin(currentTheta),
                              0.5f,
                              r * j * step * glm::cos(currentTheta));

            glm::vec3 topRight(r * j * step * glm::sin(nextTheta),
                               0.5f,
                               r * j * step * glm::cos(nextTheta));

            glm::vec3 bottomLeft(r * (j + 1) * step * glm::sin(currentTheta),
                                 0.5f,
                                 r * (j + 1) * step * glm::cos(currentTheta));

            glm::vec3 bottomRight(r * (j + 1) * step * glm::sin(nextTheta),
                                  0.5f,
                                  r * (j + 1) * step * glm::cos(nextTheta));

            makeTile(topLeft, topRight, bottomLeft, bottomRight, true);
        }
    }
}

void Cylinder::makeTile(glm::vec3 topLeft,
                        glm::vec3 topRight,
                        glm::vec3 bottomLeft,
                        glm::vec3 bottomRight, bool top) {
    // Calculate the normals based on the direction from origin to the vertex
    glm::vec3 normal(0.0f, top ? 1.0f : -1.f, 0.0f);
    // bottom-left triangle
    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, normal);
    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, normal);
    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, normal);
    // top-right triangle
    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, normal);
    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, normal);
    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, normal);

}

void Cylinder::makeBottomCap() {

    float r = 0.5f;
    float thetaStep = glm::radians(360.f / m_param2);
    for(int i = 0; i < m_param2; ++i) {
        float currentTheta = i * thetaStep;
        float nextTheta = (i + 1) * thetaStep;
        for(int j = 0; j < m_param1; ++j) {
            float step = 1.f / m_param1;

            glm::vec3 topLeft(r * j * step * glm::sin(currentTheta),
                              -0.5f,
                              r * j * step * glm::cos(currentTheta));

            glm::vec3 topRight(r * j * step * glm::sin(nextTheta),
                               -0.5f,
                               r * j * step * glm::cos(nextTheta));

            glm::vec3 bottomLeft(r * (j + 1) * step * glm::sin(currentTheta),
                                 -0.5f,
                                 r * (j + 1) * step * glm::cos(currentTheta));

            glm::vec3 bottomRight(r * (j + 1) * step * glm::sin(nextTheta),
                                  -0.5f,
                                  r * (j + 1) * step * glm::cos(nextTheta));

            makeTile(topLeft, bottomLeft, topRight, bottomRight, false);
        }
    }
}

void Cylinder::makeSideWalls() {
    float heightStep = 1.0f / m_param1;
    float thetaStep = 2 * M_PI / m_param2;

    for (int i = 0; i < m_param2; ++i) {
        float currentTheta = i * thetaStep;
        float nextTheta = (i + 1) * thetaStep;

        for (int j = 0; j < m_param1; ++j) {
            float currentY = -0.5f + j * heightStep;
            float nextY = currentY + heightStep;

            glm::vec3 topLeft(0.5f * glm::sin(currentTheta), nextY, 0.5f * glm::cos(currentTheta));
            glm::vec3 topRight(0.5f * glm::sin(nextTheta), nextY, 0.5f * glm::cos(nextTheta));
            glm::vec3 bottomLeft(0.5f * glm::sin(currentTheta), currentY, 0.5f * glm::cos(currentTheta));
            glm::vec3 bottomRight(0.5f * glm::sin(nextTheta), currentY, 0.5f * glm::cos(nextTheta));

            glm::vec3 normal_tl = glm::normalize(glm::vec3(topLeft[0], 0, topLeft[2]));
            glm::vec3 normal_tr = glm::normalize(glm::vec3(topRight[0], 0, topRight[2]));
            glm::vec3 normal_bl = glm::normalize(glm::vec3(bottomLeft[0], 0, bottomLeft[2]));
            glm::vec3 normal_br = glm::normalize(glm::vec3(bottomRight[0], 0, bottomRight[2]));

            // Two triangles for each rectangle
            insertVec3(m_vertexData, topLeft);
            insertVec3(m_vertexData, normal_tl);
            insertVec3(m_vertexData, bottomLeft);
            insertVec3(m_vertexData, normal_bl);
            insertVec3(m_vertexData, topRight);
            insertVec3(m_vertexData, normal_br);

            insertVec3(m_vertexData, topRight);
            insertVec3(m_vertexData, normal_tr);
            insertVec3(m_vertexData, bottomLeft);
            insertVec3(m_vertexData, normal_tl);
            insertVec3(m_vertexData, bottomRight);
            insertVec3(m_vertexData, normal_br);
        }
    }
}

void Cylinder::setVertexData() {
    makeTopCap(); // Top cap
    makeBottomCap(); // Bottom cap
    makeSideWalls(); // Side walls
}

void Cylinder::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}
