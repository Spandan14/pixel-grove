#include "tulip.h"

Tulip::Tulip()
{
    this->tulips.push_back(premise(this->flower_mat));
    this->tulips.push_back(premise(this->flower_mat_2));
    this->flattenLSystem(tulips, tulips_render);
}

void Tulip :: freeMeshes(){
    flower_mesh->freeMesh();
    tulip_stem->freeMesh();
}
L_node * Tulip::premise(SceneMaterial& mat){
    std::vector<L_node *> no_branches;
    L_node * bulb = new L_node{
        Flower_Component::FLOWER,
        f_step,
        mat,
        no_branches,
            flower_mesh->getSize()

    };

    std::vector<L_node *> premise_branches{rule1(2, bulb)};
    for(int i = 0; i < 2; ++i){
        L_node * leaf = new L_node{
            Flower_Component::LEAF,
            rotate_about(glm::vec4(0, 1, 0, 0), angle * i),
            this->leaf_mat,
                no_branches, leaf_mesh->getSize()

        };
        premise_branches.push_back(leaf);
    }
    L_node * start = new L_node{
        Flower_Component::STEM,
        glm::mat4(1.0),
        this->stem_mat,
        premise_branches,
        tulip_stem->getSize()
    };
    return start;
}

L_node * Tulip::rule1(int number, L_node * next){
    std::vector<L_node *> branches;
    L_node * stem = new L_node{
        Flower_Component::STEM,
        f_step,
        this->stem_mat,
        std::vector<L_node *>{number > 0 ? rule1(number - 1, next) : next},
        tulip_stem->getSize()
    };
    return stem;
}

