#ifndef TULIP_H
#define TULIP_H


#include "src/flowergen/flowers.h"
#include "../utils/scenedata.h"
#include "src/flowergen/stem.h"
#include "src/utils/mesh.h"
class tulip : public Flower
{
public:
    tulip();
private:
    Mesh* flower_mesh = new Mesh("resources/assets/tulip.obj");
    GLuint m_flower;
    GLuint m_leaf;
    SceneMaterial flower_mat {SceneColor(255, 0, 0, 0), SceneColor(255, 0, 0, 0), SceneColor(1, 1, 1, 1), 0.25, SceneColor(0, 0, 0, 0)};
    SceneMaterial leaf_mat {SceneColor(0, 255, 0, 0), SceneColor(0, 255, 0, 0), SceneColor(1, 1, 1, 1), 0.25, SceneColor(0, 0, 0, 0)};
    Stem* tulip_stem = new Stem(1, 1);

};

#endif // TULIP_H
