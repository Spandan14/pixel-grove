#ifndef ROSE_H
#define ROSE_H



#include "src/flowergen/flowers.h"
#include "../utils/scenedata.h"
#include "src/utils/mesh.h"
#include "src/flowergen/stem.h"
class Rose : public Flower
{
public:
    Rose();
    void freeMeshes() override;
    GLuint getFlowerVAO() override {return this->m_flower;}
    GLuint getStemVAO() override {return this->m_stem;}
    GLuint getLeafVAO() override {return this->m_leaf;}

    void drawRoses(GLuint m_shader, int index, glm::vec3& location) {drawMesh(m_shader, roses_render[index], location);}
private:
    //flower
    Mesh* flower_mesh = new Mesh("../resources/assets/rose.obj");
    GLuint m_flower = flower_mesh->getVAO();
    SceneMaterial flower_mat {SceneColor(0.8, 0, 0, 0), SceneColor(0.8, 0, 0, 0), SceneColor(0.2, 0.2,0.2, 1), 0.25, SceneColor(0, 0, 0, 0)};

    //leaf
    Mesh* leaf_mesh = new Mesh("../resources/assets/r_leaf.obj");
    GLuint m_leaf = leaf_mesh->getVAO();
    SceneMaterial leaf_mat {SceneColor(0, 0.6, 0.2, 0), SceneColor(0, 0.6, 0.2, 0), SceneColor(0.7, 0.7, 0.7, 1), 0.25, SceneColor(0, 0, 0, 0)};

    //stem
    float radius = 0.01;
    float height = 0.1;
    float decay = 0.7;
    Stem* rose_stem = new Stem(height, radius);
    GLuint m_stem = rose_stem->getVAO();
    SceneMaterial stem_mat {SceneColor(0, 0.6, 0.2, 0), SceneColor(0, 0.6, 0.2, 0), SceneColor(0.5, 0.5, 0.5, 1), 0.25, SceneColor(0, 0, 0, 0)};

    glm::mat4 f_step =   glm::mat4(1.0, 0.0, 0.0, 0.0,
                                   0.0, 1.0, 0.0, 0.0,
                                   0.0, 0.0, 1.0, 0.0,
                                   0.0, this->height * 0.95, 0.0, 1.0);

    //L-system
    std::vector<L_node *> roses;
    L_node * premise();
    L_node * rule1(int number = 1, L_node * next = nullptr);
    L_node * rule2();

    //turning angles
    float angle = 2 * M_PI/3;

    //rendering
    std::vector<std::vector<flowerData>> roses_render;

};

#endif // ROSE_H