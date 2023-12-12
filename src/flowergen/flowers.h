#ifndef FLOWERS_H
#define FLOWERS_H

#include <vector>
#include <glm/glm.hpp>
#include <GL/glew.h>

enum class Flower_Component{
    FLOWER,
    LEAF,
    STEM,
};

struct L_node {
    Flower_Component node_type;
    glm::mat4 transformation;
    std::vector<L_node*> children;
};

class Flower{
    public:
        virtual void genFlowers() = 0;
        virtual std::vector<GLuint> getVAOs() = 0;
        virtual void loadFlower() = 0;
        virtual void loadLeaf() = 0;
};





#endif // FLOWERS_H

