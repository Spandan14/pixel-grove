#ifndef TULIP_H
#define TULIP_H


#include "src/flowergen/flowers.h"
#include "../utils/scenedata.h"
#include "src/utils/stem.h"
#include "src/utils/mesh.h"
class Tulip : public Flower
{
public:
    Tulip();
    void genFlowers() override;
    void drawTulip(GLuint m_shader);
    GLuint getFlowerVAO() override {return this->m_flower;}
    SceneMaterial getFlowerMat() override {return this->flower_mat;}
    GLuint getStemVAO() override {return this->m_stem;}
    SceneMaterial getStemMat() override {return this->stem_mat;}
    GLuint getLeafVAO() override {return this->m_leaf;}
    SceneMaterial getLeafMat() override {return this->leaf_mat;}

    void drawTulips(GLuint m_shader) {drawMesh(m_shader, tulips_render[0]);}

private:
    //flower
    Mesh* flower_mesh = new Mesh("resources/assets/tulip.obj");
    GLuint m_flower = flower_mesh->getVAO();
    SceneMaterial flower_mat {SceneColor(1, 0, 0, 0), SceneColor(1, 0, 0, 0), SceneColor(1, 1, 1, 1), 0.25, SceneColor(0, 0, 0, 0)};

    //leaf
    GLuint m_leaf;
    SceneMaterial leaf_mat {SceneColor(0, 1, 0, 0), SceneColor(0, 1, 0, 0), SceneColor(1, 1, 1, 1), 0.25, SceneColor(0, 0, 0, 0)};

    //stem
    float radius = 0.01;
    float height = 0.1;
    float decay = 1;
    Stem* tulip_stem = new Stem(height, radius);
    GLuint m_stem = tulip_stem->getVAO();
    SceneMaterial stem_mat {SceneColor(0, 1, 0, 0), SceneColor(0, 1, 0, 0), SceneColor(1, 1, 1, 1), 0.25, SceneColor(0, 0, 0, 0)};

    glm::mat4 f_step =   glm::mat4(1.0, 0.0, 0.0, 0.0,
                                 0.0, 1.0, 0.0, 0.0,
                                 0.0, 0.0, 1.0, 0.0,
                                 0.0, this->height, 0.0, 1.0);

    //L-system
    std::vector<L_node *> tulips;
    L_node * premise();
    L_node * rule1(int number = 1, L_node * next = nullptr);

    //turning angles
    float angle = M_PI/6;

    //rendering
    std::vector<std::vector<flowerData>> tulips_render;

};

#endif // TULIP_H
