#include "Cone.h"

void Cone::updateParams(int param1, int param2) {
    m_vertexData = std::vector<float>();
    m_param1 = param1;
    m_param2 = (param2 < 3) ? 3 : param2;
    setVertexData();
}

void Cone::makeSideWalls() {
    float r = 0.5f;
    float height = 1.0f;
    glm::vec3 apex(0.0f, 0.5f, 0.0f);
    float thetaStep = 2 * M_PI / m_param2;

    for (int i = 0; i < m_param2; ++i) {
        float currentTheta = i * thetaStep;
        float nextTheta = (i + 1) * thetaStep;

        for (int j = 0; j < m_param1; ++j) {
            float frac = static_cast<float>(j) / m_param1;
            float nextFrac = static_cast<float>(j + 1) / m_param1;

            // Interpolate radius and y-coordinate
            float currentR = r * (1 - frac);
            float nextR = r * (1 - nextFrac);
            float currentY = -0.5f + height * frac;
            float nextY = -0.5f + height * nextFrac;

            // Create vertices
            glm::vec3 currentBase(currentR * glm::sin(currentTheta), currentY, currentR * glm::cos(currentTheta));
            glm::vec3 nextBase(currentR * glm::sin(nextTheta), currentY, currentR * glm::cos(nextTheta));
            glm::vec3 nextUpperBase(nextR * glm::sin(nextTheta), nextY, nextR * glm::cos(nextTheta));
            glm::vec3 currentUpperBase(nextR * glm::sin(currentTheta), nextY, nextR * glm::cos(currentTheta));

            glm::vec3 normalCurrentBase = glm::normalize(glm::vec3(2.f * currentR * glm::sin(currentTheta),
                                                                   0.25 - (currentY / 2.f),
                                                                   2.f * currentR * glm::cos(currentTheta)));
            glm::vec3 normalNextBase = glm::normalize(glm::vec3(2.f * currentR * glm::sin(nextTheta),
                                                                0.25 - (currentY / 2.f),
                                                                2.f * currentR * glm::cos(nextTheta)));

            glm::vec3 normalnextUpperBase = normalNextBase;
            glm::vec3 normalcurrentUpperBase = normalCurrentBase;
            if(nextR == 0){
                float theta = (nextTheta - currentTheta) / 2.f;
                normalcurrentUpperBase = glm::normalize(glm::vec3(normalCurrentBase[0] * glm::cos(theta) + normalCurrentBase[2] * glm::sin(theta),
                                                                  normalCurrentBase[1],
                                                                  -normalCurrentBase[0] * glm::sin(theta) + normalCurrentBase[2] * glm::cos(theta)));
                normalnextUpperBase = glm::normalize(glm::vec3(normalNextBase[0] * glm::cos(theta) + normalNextBase[2] * glm::sin(theta),
                                                               normalNextBase[1],
                                                               -normalNextBase[0] * glm::sin(theta) + normalNextBase[2] * glm::cos(theta)));
            }

            // Create triangles

            insertVec3(m_vertexData, currentUpperBase);
            insertVec3(m_vertexData, normalcurrentUpperBase);
            insertVec3(m_vertexData, currentBase);
            insertVec3(m_vertexData, normalCurrentBase);
            insertVec3(m_vertexData, nextBase);
            insertVec3(m_vertexData, normalNextBase);

            insertVec3(m_vertexData, currentUpperBase);
            insertVec3(m_vertexData, normalcurrentUpperBase);
            insertVec3(m_vertexData, nextBase);
            insertVec3(m_vertexData, normalNextBase);
            insertVec3(m_vertexData, nextUpperBase);
            insertVec3(m_vertexData, normalnextUpperBase);
        }
    }
}

void Cone::makeBase() {
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

void Cone::makeTile(glm::vec3 topLeft,
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

void Cone::setVertexData() {
    makeBase();
    makeSideWalls();
}

void Cone::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}
