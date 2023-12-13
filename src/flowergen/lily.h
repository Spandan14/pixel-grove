#ifndef LILY_H
#define LILY_H


#include "src/flowergen/flowers.h"
#include "src/utils/mesh.h"
#include "src/utils/stem.h"
class Lily : public Flower
{
public:
    Lily();
    void freeMeshes() override;
    GLuint getFlowerVAO() override {return this->m_flower;}
    SceneMaterial getFlowerMat() override {return this->flower_mat;}
    GLuint getStemVAO() override {return this->m_stem;}
    SceneMaterial getStemMat() override {return this->stem_mat;}
    GLuint getLeafVAO() override {return this->m_leaf;}
    SceneMaterial getLeafMat() override {return this->leaf_mat;}

    void drawLilies(GLuint m_shader) {drawMesh(m_shader, lilies_render[0]);}

private:
    //flower
    Mesh* flower_mesh = new Mesh("resources/assets/tulip.obj");
    GLuint m_flower = flower_mesh->getVAO();
    SceneMaterial flower_mat {SceneColor(0.98, 0.98, 0.8, 0), SceneColor(0.98, 0.98, 0.8, 0), SceneColor(0, 0,0, 1), 0.25, SceneColor(0, 0, 0, 0)};

    //leaf
    Mesh* leaf_mesh = new Mesh("resources/assets/t_leaf.obj");
    GLuint m_leaf = leaf_mesh->getVAO();
    SceneMaterial leaf_mat {SceneColor(0, 0.8, 0, 0), SceneColor(0, 0.8, 0, 0), SceneColor(0.7, 0.7, 0.7, 1), 0.25, SceneColor(0, 0, 0, 0)};

    //stem
    float radius = 0.01;
    float height = 0.1;
    Stem* lily_stem = new Stem(height, radius);
    GLuint m_stem = lily_stem->getVAO();
    SceneMaterial stem_mat {SceneColor(0.45, 0.84, 0.45, 0), SceneColor(0.45, 0.84, 0.45, 0), SceneColor(0.5, 0.5, 0.5, 1), 0.25, SceneColor(0, 0, 0, 0)};

    glm::mat4 f_step =   glm::mat4(1.0, 0.0, 0.0, 0.0,
                                 0.0, 1.0, 0.0, 0.0,
                                 0.0, 0.0, 1.0, 0.0,
                                 0.0, this->height* 0.95, 0.0, 1.0);

    //L-system
    std::vector<L_node *> lilies;
    L_node * premise();
    L_node * rule1(int number = 1);
    L_node * rule2(int number, L_node * next);

    //turning angles
    float angle = 2 * M_PI/3;

    //rendering
    std::vector<std::vector<flowerData>> lilies_render;
};

#endif // LILY_H
