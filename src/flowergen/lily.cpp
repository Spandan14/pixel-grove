#include "lily.h"
#include "src/utils/ctm_utils.h"

Mesh* Lily::LILY_FLOWER_MESH = nullptr;
Mesh* Lily::LILY_LEAF_MESH = nullptr;
Stem* Lily::LILY_STEM_MESH = nullptr;

float Lily::LILY_STEM_RADIUS = 0.005;
float Lily::LILY_STEM_HEIGHT = 0.05;

SceneMaterial Lily::LILY_FLOWER_MAT = SceneMaterial{SceneColor(0.98, 0.98, 0.8, 0), SceneColor(0.98, 0.98, 0.8, 0),
                                              SceneColor(0.2, 0.2, 0.2, 1), 0.25, SceneColor(0, 0, 0, 0)};
SceneMaterial Lily::LILY_LEAF_MAT = SceneMaterial{SceneColor(0, 0.8, 0, 0), SceneColor(0, 0.8, 0, 0),
                                                   SceneColor(0.7, 0.7, 0.7, 1), 0.25, SceneColor(0, 0, 0, 0)};
SceneMaterial Lily::LILY_STEM_MAT = SceneMaterial{SceneColor(0.45, 0.84, 0.45, 0), SceneColor(0.45, 0.84, 0.45, 0),
                                            SceneColor(0.5, 0.5, 0.5, 1), 0.25, SceneColor(0, 0, 0, 0)};

glm::mat4 Lily::LILY_FLOWER_STEP = glm::mat4(1.0, 0.0, 0.0, 0.0,
                                              0.0, 1.0, 0.0, 0.0,
                                              0.0, 0.0, 1.0, 0.0,
                                              0.0, LILY_STEM_HEIGHT * 0.95, 0.0, 1.0);
float Lily::LILY_ANGLE = 2 * M_PI / 3;

Lily::Lily() {
    // init statics
    Lily::LILY_FLOWER_MESH = new Mesh("../resources/assets/lily.obj");
    Lily::LILY_LEAF_MESH = new Mesh("../resources/assets/l_leaf.obj");
    Lily::LILY_STEM_MESH = new Stem(LILY_STEM_HEIGHT, LILY_STEM_RADIUS);


    this->flower = premise();
    this->flattenLSystem(this->flower, flowerData);
}

LSystemNode *Lily::premise() {
    return rule2(4, rule1(4));
}

LSystemNode *Lily::rule1(int number) {
    std::vector<LSystemNode *> branches;
    LSystemNode *flower = new LSystemNode{
            FlowerComponent::FLOWER,
            LILY_FLOWER_STEP * rotate_about(glm::vec4(1, 0, 0, 0), LILY_ANGLE / 2) * glm::mat4(glm::mat3(0.6)),
            Lily::LILY_FLOWER_MAT,
            std::vector<LSystemNode *>{},
            LILY_FLOWER_MESH->getSize()
    };
    LSystemNode *flower_2 = new LSystemNode{
            FlowerComponent::FLOWER,
            Lily::LILY_FLOWER_STEP * rotate_about(glm::vec4(1, 0, 0, 0), -LILY_ANGLE / 2) * glm::mat4(glm::mat3(0.6)),
            Lily::LILY_FLOWER_MAT,
            std::vector<LSystemNode *>{},
            LILY_FLOWER_MESH->getSize()
    };
    for (int i = 0; i < 2; ++i) {

        LSystemNode *branch = new LSystemNode{
                FlowerComponent::STEM,
                LILY_FLOWER_STEP * rotate_about(glm::vec4(1, 0, 0, 0), LILY_ANGLE - 2 * i * LILY_ANGLE + M_PI) *
                glm::mat4(glm::mat3(0.6)),
                Lily::LILY_STEM_MAT,
                std::vector<LSystemNode *>{i % 2 == 0 ? flower_2 : flower},
                LILY_STEM_MESH->getSize()
        };
        branches.push_back(branch);
    }
    if (number > 0) {
        branches.push_back(rule1(number - 1));
    }
    LSystemNode *stem = new LSystemNode{
            FlowerComponent::STEM,
            LILY_FLOWER_STEP * rotate_about(glm::vec4(0, 1, 0, 0), LILY_ANGLE),
            Lily::LILY_STEM_MAT,
            branches,
            LILY_STEM_MESH->getSize()
    };
    return stem;
}

LSystemNode *Lily::rule2(int number, LSystemNode *next, bool start) {
    std::vector<LSystemNode *> branches;
    for (int i = 0; i < 2; ++i) {

        LSystemNode *branch = new LSystemNode{
                FlowerComponent::LEAF,
                LILY_FLOWER_STEP * rotate_about(glm::vec4(0, 1, 0, 0), M_PI * i) * glm::mat4(glm::mat3(0.36)),
                Lily::LILY_LEAF_MAT,
                std::vector<LSystemNode *>{},
                LILY_LEAF_MESH->getSize()
        };
        branches.push_back(branch);
    }
    if (number > 0) {
        branches.push_back(rule2(number - 1, next, false));
    } else {
        branches.push_back(next);
    }
    LSystemNode* stem = new LSystemNode{
            FlowerComponent::STEM,
            (start ? glm::mat4(1.0) : LILY_FLOWER_STEP) * rotate_about(glm::vec4(0, 1, 0, 0), LILY_ANGLE),
            Lily::LILY_STEM_MAT,
            branches,
            LILY_STEM_MESH->getSize()
    };
    return stem;
}

std::vector<float> Lily::getFlowerVBO() {
    return LILY_FLOWER_MESH->getTriangles();
}

std::vector<float> Lily::getStemVBO() {
    return std::vector<float>();
}

std::vector<float> Lily::getLeafVBO() {
    return std::vector<float>();
}
