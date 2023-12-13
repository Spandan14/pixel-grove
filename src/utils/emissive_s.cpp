#include "emissive_s.h"
#include <iostream>

Emissive_S::Emissive_S(glm::vec4 location, std::vector<SceneLightData>& lights){
    this->location = location;
    this->setVertexData();
    glGenBuffers(1, &m_vbo);
    glGenVertexArrays(1, &m_vao);
    this->s_light.type = LightType::LIGHT_POINT;
    this->s_light.pos = location;
    this->s_light.color = glm::vec4(1.f, 1.f, 0.75f, 1.f);
    this->s_light.function = glm::vec3(0.5, 0.5, 0.5);

    this->CTM = glm::mat4(1.f, 0.f, 0.f, 0.f,
                          0.f, 1.f, 0.f, 0.f,
                          0.f, 0.f, 1.f, 0.f,
                          this->location[0], this->location[1], this->location[2], 1.f);
    this->iCTM = glm::transpose(glm::inverse(glm::mat3(CTM)));

    sphereMaterial.cEmissive = glm::vec4(1.f, 1.f, 0.75f, 1.f);
    sphereMaterial.emissive = true;
    sphereMaterial.cAmbient = glm::vec4(1.f);
    sphereMaterial.cDiffuse = glm::vec4(0.f);
    sphereMaterial.cSpecular = glm::vec4(1.f);
    sphereMaterial.shininess = 20.f;


    lights.push_back(s_light);
    std::cout<<m_vertexData.size()<<std::endl;
    this->bindMesh();
}

glm::mat4 Emissive_S::getCTM(){
    return this->CTM;
}

glm::mat3 Emissive_S::getiCTM(){
    return this->iCTM;
}

void Emissive_S::freeMesh(){
    glDeleteBuffers(1, &m_vbo);
    glDeleteVertexArrays(1, &m_vao);
}

void Emissive_S::passUniforms(GLuint m_shader){
    GLint shininess_Location = glGetUniformLocation(m_shader, "shininess");
    glUniform1f(shininess_Location, sphereMaterial.shininess);

    GLint ambient_Location = glGetUniformLocation(m_shader, "ambientColor");
    glUniform4fv(ambient_Location, 1, &sphereMaterial.cAmbient[0]);

    GLint diffuse_Location = glGetUniformLocation(m_shader, "diffuseColor");
    glUniform4fv(diffuse_Location, 1, &sphereMaterial.cDiffuse[0]);

    GLint specular_Location = glGetUniformLocation(m_shader, "specularColor");
    glUniform4fv(specular_Location, 1, &sphereMaterial.cSpecular[0]);

    GLint emissive_Location = glGetUniformLocation(m_shader, "emissive");
    glUniform4fv(emissive_Location, 1, &sphereMaterial.cEmissive[0]);

    GLint emisbool_l = glGetUniformLocation(m_shader, "emisbool");
    glUniform1i(emisbool_l, sphereMaterial.emissive);

    GLint modelLocation = glGetUniformLocation(m_shader, "modelMatrix");
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, &CTM[0][0]);

    GLint inverseCTMLocation = glGetUniformLocation(m_shader, "inverseCTM");
    glUniformMatrix3fv(inverseCTMLocation, 1, GL_FALSE, &iCTM[0][0]);
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
    float r = 0.05f;
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
