#ifndef STEM_H
#define STEM_H

#include "src/utils/drawables.h"
#include <vector>
#include <glm/glm.hpp>
#include <GL/glew.h>


class Stem : public Drawables {
public:
    Stem(float height, float radius);

    void genStem();

    void freeMesh() override;

    int getSize() override { return m_stem.size() / 6; }

    GLuint getVAO();

    void drawMesh() override {
        glBindVertexArray(this->m_vao);
        glDrawArrays(GL_TRIANGLES, 0, this->m_stem.size() / 6);
        glBindVertexArray(0);
    };

    std::vector<float> getTriangles() {return m_stem;};

    void bind() {
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * m_stem.size(), m_stem.data(), GL_STATIC_DRAW);
        glBindVertexArray(m_vao);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), 0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat),
                              reinterpret_cast<void *>(3 * sizeof(GLfloat)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

private:

    void insertVec3(std::vector<float> &data, glm::vec3 v) {
        data.push_back(v.x);
        data.push_back(v.y);
        data.push_back(v.z);
    }

    void insertVec2(std::vector<float> &data, glm::vec2 v) {
        data.push_back(v.x);
        data.push_back(v.y);
    }

    void setVertexData();

    void makeWedge(float currTheta, float nextTheta);

    void makeTile_cap(glm::vec3 topLeft,
                      glm::vec3 topRight,
                      glm::vec3 bottomLeft,
                      glm::vec3 bottomRight,
                      bool up);

    void makeTile_face(glm::vec3 topLeft,
                       glm::vec3 topRight,
                       glm::vec3 bottomLeft,
                       glm::vec3 bottomRight);

    glm::vec2 getCylinderFaceUV(glm::vec3 point);
    glm::vec2 getCylinderCapUV(glm::vec3 point, bool up);

    float height;
    float radius;
    GLuint m_vbo;
    GLuint m_vao;
    std::vector<float> m_stem;

};

#endif // STEM_H
