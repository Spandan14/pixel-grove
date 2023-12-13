#include "rose.h"

Rose::Rose()
{
    this->roses.push_back(premise());
    this->flattenLSystem(roses, roses_render);
}

void Rose :: freeMeshes(){
    flower_mesh->freeMesh();
    rose_stem->freeMesh();
}

L_node * Rose::premise(){
    std::vector<L_node *> no_branches;
    L_node * final_flower = new L_node{
        Flower_Component::FLOWER,
        f_step,
        no_branches,
        flower_mesh->getSize()

    };

    std::vector<L_node *> next{        new L_node{
            Flower_Component::STEM,
                f_step,
                std::vector<L_node *>{rule1(1, final_flower)},
                rose_stem->getSize()
    }};

    L_node * start = new L_node {
        Flower_Component::STEM,
        rotate_about(glm::vec4(0, 0, 1, 0), -angle/10),
        next,
        rose_stem->getSize()};
    return start;
}

L_node * Rose::rule1(int number, L_node * next){
    std::vector<L_node *> branches{ next };
    for(unsigned int i = 0; i < 3; ++i){
        glm::mat4 transform = rotate_about(glm::vec4(0, 1, 0, 0), angle * i)  * rotate_about(glm::vec4(1, 0, 0, 0), angle/3) * glm::mat4(glm::mat3(decay));
        L_node * branch = new L_node{
            Flower_Component::STEM,
            transform,
            std::vector<L_node *> {number > 0 ? rule1(number - 1, next) : next},
            rose_stem->getSize()
        };
        branches.push_back(branch);
    }
    L_node * stem = new L_node{
        Flower_Component::STEM,
        f_step * rotate_about(glm::vec4(0, 0, 1, 0), angle/5),
        branches,
        rose_stem->getSize()
    };
    return stem;
}
