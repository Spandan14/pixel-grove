#include "stem.h"

Stem::Stem(float height, float radius)
{
    this->height = height;
    this->radius = radius;
    glGenBuffers(1, &m_vbo);
    glGenVertexArrays(1, &m_vao);
    this->genStem();
}


void Stem::genStem(){

    this->setVertexData();
}

void Stem::freeBindings(){
    glDeleteBuffers(1, &m_vbo);
    glDeleteVertexArrays(1, &m_vao);
}


GLuint Stem::getVAO(){
    return this->m_vao;
}


void Stem::setVertexData() {
    // TODO for Project 5: Lights, Camera
    float step = glm::radians(360.f/10);
    for(int i = 0; i < 10; ++i){
        makeWedge(step * i, step * (i + 1));
    }

}

void Stem::makeWedge(float currTheta, float nextTheta){
    //top triangle?
    //vector for leg

    float step = 1;

    for(int i = 0; i < step; ++i){

        //top
        glm::vec3 tL = glm::vec3(this->radius * step * i * glm::sin(currTheta),
                                 this->height,
                                 this->radius * step * i * glm::cos(currTheta));
        glm::vec3 tR = glm::vec3(this->radius * step * i * glm::sin(nextTheta),
                                 this->height,
                                 this->radius *  step * i * glm::cos(nextTheta));
        glm::vec3 bL = glm::vec3(this->radius *  step * (i + 1) * glm::sin(currTheta),
                                 this->height,
                                 this->radius *  step * (i + 1) * glm::cos(currTheta));
        glm::vec3 bR = glm::vec3(this->radius * step * (i + 1) *glm::sin(nextTheta),
                                 this->height,
                                 this->radius *step * (i + 1) * glm::cos(nextTheta));
        makeTile_cap(tL, tR, bL, bR, true);

        //bottom
        glm::vec3 tLd = glm::vec3(this->radius * step * i * glm::sin(nextTheta),
                                  0,
                                  this->radius *  step * i * glm::cos(nextTheta));
        glm::vec3 tRd = glm::vec3(this->radius * step * i * glm::sin(currTheta),
                                  0,
                                  this->radius * step * i * glm::cos(currTheta));
        glm::vec3 bLd = glm::vec3(this->radius * step * (i + 1) *glm::sin(nextTheta),
                                  0,
                                  this->radius *step * (i + 1) * glm::cos(nextTheta));
        glm::vec3 bRd = glm::vec3(this->radius *  step * (i + 1) * glm::sin(currTheta),
                                  0,
                                  this->radius *  step * (i + 1) * glm::cos(currTheta));
        makeTile_cap(tLd, tRd, bLd, bRd, false);

        //side
        glm::vec3 tLs = glm::vec3(this->radius * glm::sin(currTheta),
                                  this->height - i * step * height,
                                  this->radius * glm::cos(currTheta));
        glm::vec3 tRs = glm::vec3(this->radius * glm::sin(nextTheta),
                                  this->height - i * step * height,
                                  this->radius * glm::cos(nextTheta));

        glm::vec3 bLs = glm::vec3(this->radius * glm::sin(currTheta),
                                  this->height - (i + 1) * step * height,
                                  this->radius * glm::cos(currTheta));
        glm::vec3 bRs = glm::vec3(this->radius * glm::sin(nextTheta),
                                  this->height - (i + 1) * step * height,
                                  this->radius * glm::cos(nextTheta));

        makeTile_face(tLs, tRs, bLs, bRs);

    }
}

void Stem::makeTile_cap(glm::vec3 topLeft,
                            glm::vec3 topRight,
                            glm::vec3 bottomLeft,
                            glm::vec3 bottomRight,
                            bool up){
    glm::vec3 normal = up ? glm::vec3(0, 1, 0) : glm::vec3(0, -1, 0);

    insertVec3(m_stem, topLeft);
    insertVec3(m_stem, normal);
    insertVec3(m_stem, bottomLeft);
    insertVec3(m_stem, normal);
    insertVec3(m_stem, bottomRight);
    insertVec3(m_stem, normal);
    insertVec3(m_stem, topLeft);
    insertVec3(m_stem, normal);
    insertVec3(m_stem, bottomRight);
    insertVec3(m_stem, normal);
    insertVec3(m_stem, topRight);
    insertVec3(m_stem, normal);

}
void Stem::makeTile_face(glm::vec3 topLeft,
                             glm::vec3 topRight,
                             glm::vec3 bottomLeft,
                             glm::vec3 bottomRight){
    glm::vec3 normal_tl = glm::normalize(glm::vec3(topLeft[0], 0, topLeft[2]));
    glm::vec3 normal_tr = glm::normalize(glm::vec3(topRight[0], 0, topRight[2]));
    glm::vec3 normal_bl = glm::normalize(glm::vec3(bottomLeft[0], 0, bottomLeft[2]));
    glm::vec3 normal_br = glm::normalize(glm::vec3(bottomRight[0], 0, bottomRight[2]));


    insertVec3(m_stem, topLeft);
    insertVec3(m_stem, normal_tl);
    insertVec3(m_stem, bottomLeft);
    insertVec3(m_stem, normal_bl);
    insertVec3(m_stem, bottomRight);
    insertVec3(m_stem, normal_br);
    insertVec3(m_stem, topLeft);
    insertVec3(m_stem, normal_tl);
    insertVec3(m_stem, bottomRight);
    insertVec3(m_stem, normal_br);
    insertVec3(m_stem, topRight);
    insertVec3(m_stem, normal_tr);


}
