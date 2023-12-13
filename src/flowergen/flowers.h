#ifndef FLOWERS_H
#define FLOWERS_H

#include "src/utils/scenedata.h"
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <GL/glew.h>

enum class FlowerComponent {
    FLOWER,
    LEAF,
    STEM,
};
//linked list structure
struct LSystemNode {
    FlowerComponent type;
    glm::mat4 ctm{};
    SceneMaterial &mat;
    std::vector<LSystemNode *> children;
    int size;
};

struct FlowerData {
    glm::mat4 ctm{};
    glm::mat3 ictm{};
    SceneMaterial mat{};
    std::vector<float> triangles;
    FlowerComponent type;
    int size{};
};

struct FlowerCTMCollection {
    std::vector<glm::mat4> leafCTMs;
    std::vector<glm::mat3> leafInverseCTMS;
    std::vector<glm::mat4> flowerCTMs;
    std::vector<glm::mat3> flowerInverseCTMS;
    std::vector<glm::mat4> stemCTMs;
    std::vector<glm::mat3> stemInverseCTMS;
};

class Flower {
public:
    virtual void flattenLSystem(LSystemNode* &root, std::vector<FlowerData> &flattened);

    void drawMesh(GLuint m_shader, std::vector<FlowerData> &flowers, glm::vec3 &location);

    virtual std::vector<float> getFlowerVBO() = 0;
    virtual std::vector<float> getStemVBO() = 0;
    virtual std::vector<float> getLeafVBO() = 0;

    void nodeDFS(LSystemNode *flower_data, std::vector<FlowerData> &to_render, glm::mat4 ctm = glm::mat4{1.0});

    static FlowerCTMCollection collectCTMs(std::vector<FlowerData> &objects);
};


#endif // FLOWERS_H

