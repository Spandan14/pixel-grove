#include "Cube.h"

void Cube::updateParams(int param1) {
    m_vertexData = std::vector<float>();
    m_param1 = param1;
    setVertexData();
}

void Cube::makeTile(glm::vec3 topLeft,
                    glm::vec3 topRight,
                    glm::vec3 bottomLeft,
                    glm::vec3 bottomRight) {
    glm::vec3 normal_bottomLeft(glm::normalize(glm::cross(bottomLeft - topLeft, bottomRight - topLeft)));
    glm::vec3 normal_topRight(glm::normalize(glm::cross(topLeft - topRight, bottomRight - topRight)));

    //bottom left triangle
    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, normal_bottomLeft);
    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, normal_bottomLeft);
    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, normal_bottomLeft);
    //top right triangle
    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, normal_topRight);
    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, normal_topRight);
    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, normal_topRight);
}

void Cube::makeFace(glm::vec3 topLeft,
                    glm::vec3 topRight,
                    glm::vec3 bottomLeft,
                    glm::vec3 bottomRight) {
    glm::vec3 horizontalStep = (topRight - topLeft) / float(m_param1);
    glm::vec3 verticalStep = (bottomLeft - topLeft) / float(m_param1);

    for (int i = 0; i < m_param1; ++i) {
        for (int j = 0; j < m_param1; ++j) {

            // Determine the four corners of the current tessellated segment
            glm::vec3 localTopLeft = topLeft + float(j) * horizontalStep + float(i) * verticalStep;
            glm::vec3 localTopRight = localTopLeft + horizontalStep;
            glm::vec3 localBottomLeft = localTopLeft + verticalStep;
            glm::vec3 localBottomRight = localBottomLeft + horizontalStep;

            // Create the triangles for this segment using makeTile()
            makeTile(localTopLeft, localTopRight, localBottomLeft, localBottomRight);
        }
    }

}

void Cube::setVertexData() {
    makeFace(glm::vec3(-0.5f,  0.5f, 0.5f),
             glm::vec3(0.5f,  0.5f, 0.5f),
             glm::vec3(-0.5f, -0.5f, 0.5f),
             glm::vec3( 0.5f, -0.5f, 0.5f));

    makeFace(glm::vec3(-0.5f,  0.5f, -0.5f),
             glm::vec3(-0.5f,  0.5f, 0.5f),
             glm::vec3(-0.5f, -0.5f, -0.5f),
             glm::vec3(-0.5f, -0.5f, 0.5f));

    //top face
    makeFace(glm::vec3(-0.5f,  0.5f, -0.5f),
             glm::vec3(0.5f,  0.5f, -0.5f),
             glm::vec3(-0.5f,  0.5f, 0.5f),
             glm::vec3(0.5f,  0.5f, 0.5f));

    makeFace(glm::vec3(0.5f,  0.5f, 0.5f),
             glm::vec3(0.5f,  0.5f, -0.5f),
             glm::vec3(0.5f, -0.5f, 0.5f),
             glm::vec3(0.5f, -0.5f, -0.5f));

    //bottom face
    makeFace(glm::vec3(-0.5f, -0.5f, 0.5f),
             glm::vec3(0.5f, -0.5f, 0.5f),
             glm::vec3(-0.5f, -0.5f, -0.5f),
             glm::vec3(0.5f,  -0.5f, -0.5f));

    makeFace(glm::vec3(0.5f,  0.5f, -0.5f),
             glm::vec3(-0.5f,  0.5f, -0.5f),
             glm::vec3(0.5f, -0.5f, -0.5f),
             glm::vec3(-0.5f, -0.5f, -0.5f));
}

void Cube::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}
