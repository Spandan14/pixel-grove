#ifndef TULIP_H
#define TULIP_H


#include "src/flowergen/flowers.h"
#include "../utils/scenedata.h"
#include "src/flowergen/stem.h"
#include "src/utils/mesh.h"
class Tulip : public Flower
{
public:
    Tulip();
    void freeMeshes() override;
    GLuint getFlowerVAO() override {return this->m_flower;}
    GLuint getStemVAO() override {return this->m_stem;}
    GLuint getLeafVAO() override {return this->m_leaf;}

    void drawTulips(GLuint m_shader, int index, glm::vec3& location) {drawMesh(m_shader, tulips_render[index], location);}

private:
    //flower
    Mesh* flower_mesh = new Mesh("../resources/assets/tulip.obj");
    GLuint m_flower = flower_mesh->getVAO();
    SceneMaterial flower_mat {SceneColor(0.98, 0.59, 0.39, 0), SceneColor(0.98, 0.59, 0.39, 0), SceneColor(0.2, 0.2,0.2, 1), 0.25, SceneColor(0, 0, 0, 0)};
    SceneMaterial flower_mat_2 {SceneColor(1, 0.71, 0.76, 0), SceneColor(1, 0.71, 0.76, 0), SceneColor(0, 0,0, 1), 0.25, SceneColor(0, 0, 0, 0)};

    //leaf
    Mesh* leaf_mesh = new Mesh("../resources/assets/t_leaf.obj");
    GLuint m_leaf = leaf_mesh->getVAO();
    SceneMaterial leaf_mat {SceneColor(0, 0.8, 0, 0), SceneColor(0, 0.8, 0, 0), SceneColor(0.7, 0.7, 0.7, 1), 0.25, SceneColor(0, 0, 0, 0)};

    //stem
    float radius = 0.01;
    float height = 0.1;
    Stem* tulip_stem = new Stem(height, radius);
    GLuint m_stem = tulip_stem->getVAO();
    SceneMaterial stem_mat {SceneColor(0.45, 0.84, 0.45, 0), SceneColor(0.45, 0.84, 0.45, 0), SceneColor(0.5, 0.5, 0.5, 1), 0.25, SceneColor(0, 0, 0, 0)};

    glm::mat4 f_step =   glm::mat4(1.0, 0.0, 0.0, 0.0,
                                 0.0, 1.0, 0.0, 0.0,
                                 0.0, 0.0, 1.0, 0.0,
                                 0.0, this->height, 0.0, 1.0);

    //L-system
    std::vector<L_node *> tulips;
    L_node * premise(SceneMaterial& color);
    L_node * rule1(int number = 1, L_node * next = nullptr);

    //turning angles
    float angle = 5 * M_PI/6;

    //rendering
    std::vector<std::vector<flowerData>> tulips_render;

};

#endif // TULIP_H
