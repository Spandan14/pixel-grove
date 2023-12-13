#ifndef FLOWERS_H
#define FLOWERS_H

#include "src/utils/scenedata.h"
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <GL/glew.h>

enum class Flower_Component{
    FLOWER,
    LEAF,
    STEM,
};
//linked list structure
struct L_node {
    Flower_Component type;
    glm::mat4 transformation;
    std::vector<L_node*> children;
    int size;
};

struct flowerData{
    glm::mat4 transformation;
    SceneMaterial mat;
    GLuint vao;
    int size;
};

class Flower{
    public:
    virtual void freeMeshes() = 0;
        virtual void genFlowers() = 0;
        virtual void flattenLSystem(std::vector<L_node *>& original, std::vector<std::vector<flowerData>> & flattened){
            for(int i = 0; i < original.size(); ++i){
                std::vector<flowerData> flattened_flower;
                nodeDFS(original[i], flattened_flower);
                flattened.push_back(flattened_flower);
            }
        }
        void drawMesh(GLuint m_shader, std::vector<flowerData>& flowers){
            for(int i = 0; i < flowers.size(); ++i){

                GLint shininess_Location = glGetUniformLocation(m_shader, "shininess");
                glUniform1f(shininess_Location, 22.f);
                GLint ambient_Location = glGetUniformLocation(m_shader, "ambientColor");
                glUniform4fv(ambient_Location, 1, &flowers[i].mat.cAmbient[0]);

                GLint diffuse_Location = glGetUniformLocation(m_shader, "diffuseColor");
                glUniform4fv(diffuse_Location, 1, &flowers[i].mat.cDiffuse[0]);

                GLint specular_Location = glGetUniformLocation(m_shader, "specularColor");
                glUniform4fv(specular_Location, 1, &flowers[i].mat.cSpecular[0]);

                GLint modelLocation = glGetUniformLocation(m_shader, "modelMatrix");
                glUniformMatrix4fv(modelLocation, 1, GL_FALSE, &flowers[i].transformation[0][0]);

                glm::mat3 inverseCTM = glm::transpose(glm::inverse(glm::mat3(flowers[i].transformation)));
                GLint inverseCTMLocation = glGetUniformLocation(m_shader, "inverseCTM");
                glUniformMatrix3fv(inverseCTMLocation, 1, GL_FALSE, &inverseCTM[0][0]);

                glBindVertexArray(flowers[i].vao);
                glDrawArrays(GL_TRIANGLES, 0, flowers[i].size);
                glBindVertexArray(0);
            }
        }
        virtual GLuint getFlowerVAO() = 0;
        virtual SceneMaterial getFlowerMat()= 0;
        virtual GLuint getStemVAO() = 0;
        virtual SceneMaterial getStemMat()= 0;
        virtual GLuint getLeafVAO() = 0;
        virtual SceneMaterial getLeafMat() = 0;
        glm::mat4 rotate_about(glm::vec4 axis, float theta){
            glm::mat4 rotate = glm::mat4(
                glm::cos(theta) + pow(axis[0], 2) * (1 - glm::cos(theta)), axis[1] * axis[0] * (1 - glm::cos(theta)) + axis[2] * glm::sin(theta), axis[0] * axis[2] * (1 - glm::cos(theta)) - axis[1] * glm::sin(theta), 0,
                axis[0] * axis[1] * (1 - glm::cos(theta)) - axis[2] * glm::sin(theta), glm::cos(theta) +  pow(axis[1], 2) * (1 - glm::cos(theta)), axis[1] * axis[2] * (1 - glm::cos(theta)) + axis[0] * glm::sin(theta), 0,
                axis[0] * axis[2] * (1 - glm::cos(theta)) + axis[1] * glm::sin(theta), axis[1] * axis[2] * (1 - glm::cos(theta)) - axis[0] * glm::sin(theta),  glm::cos(theta) + pow(axis[2], 2) * (1 - glm::cos(theta)), 0,
                0, 0, 0, 1);
            return rotate;
        }

        void nodeDFS(L_node * flower_data, std::vector<flowerData>& to_render, glm::mat4 ctm = glm::mat4{1.0}){
            ctm *= flower_data->transformation;
            flowerData parsed;
            parsed.transformation = ctm;

            parsed.size = flower_data->size;
            switch(flower_data->type){

            case(Flower_Component::FLOWER):
                parsed.mat = getFlowerMat();
                parsed.vao = getFlowerVAO();
                break;
            case(Flower_Component::STEM):
                parsed.mat = getStemMat();
                parsed.vao = getStemVAO();
                break;
            case(Flower_Component::LEAF):
                parsed.mat = getLeafMat();
                parsed.vao = getLeafVAO();
                break;
            }
            to_render.push_back(parsed);
            for(unsigned int i = 0; i < flower_data->children.size(); ++i){
                nodeDFS(flower_data->children[i], to_render, ctm);
            }
        }
};





#endif // FLOWERS_H

