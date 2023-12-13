#include "tulip.h"

Tulip::Tulip()
{
    this->tulips.push_back(premise());
    this->flattenLSystem(tulips, tulips_render);
}

void Tulip :: freeMeshes(){
    flower_mesh->freeMesh();
    tulip_stem->freeMesh();
}

void Tulip :: genFlowers(){
    this->tulips.push_back(premise());
    this->flattenLSystem(tulips, tulips_render);
}

L_node * Tulip::premise(){
    std::vector<L_node *> no_branches;
    L_node * bulb = new L_node{
        Flower_Component::FLOWER,
        f_step,
        no_branches,
            flower_mesh->getSize()

    };
    L_node * start = new L_node{
        Flower_Component::STEM,
            glm::mat4(1.0),
            std::vector<L_node *>{rule1(2, bulb)},
        tulip_stem->getSize()
    };
    return start;
}

L_node * Tulip::rule1(int number, L_node * next){
    std::vector<L_node *> branches;
    std::cout<<number<<std::endl;
    L_node * stem = new L_node{
        Flower_Component::STEM,
            f_step,
        std::vector<L_node *>{number > 0 ? rule1(number - 1, next) : next},
        tulip_stem->getSize()
    };
    return stem;
}

