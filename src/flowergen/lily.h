#ifndef LILY_H
#define LILY_H


#include "src/flowergen/flowers.h"
#include "src/utils/mesh.h"
#include "src/flowergen/stem.h"

class Lily : public Flower {
public:
    static Mesh *LILY_FLOWER_MESH;
    static Mesh *LILY_LEAF_MESH;

    static float LILY_STEM_RADIUS;
    static float LILY_STEM_HEIGHT;
    static Stem *LILY_STEM_MESH;

    static SceneMaterial LILY_FLOWER_MAT;
    static SceneMaterial LILY_LEAF_MAT;
    static SceneMaterial LILY_STEM_MAT;

    static glm::mat4 LILY_FLOWER_STEP;
    static float LILY_ANGLE;

    Lily();

    std::vector<float> getFlowerVBO() override;
    std::vector<float> getStemVBO() override;
    std::vector<float> getLeafVBO() override;

    LSystemNode *flower;
    std::vector<FlowerData> flowerData;

private:
    LSystemNode *premise();
    LSystemNode *rule1(int number = 1);
    LSystemNode *rule2(int number, LSystemNode *next, bool start = true);
};

#endif // LILY_H
