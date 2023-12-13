#include "lily.h"

Lily::Lily()
{
    this->lilies.push_back(premise());
    this->flattenLSystem(lilies, lilies_render);
}

void Lily :: freeMeshes(){
    flower_mesh->freeMesh();
    lily_stem->freeMesh();
}
L_node * Lily::premise(){

    return rule2(4, rule1(4));
}

L_node * Lily::rule1(int number){
    std::vector<L_node *> branches;
    L_node * flower = new L_node{
        Flower_Component::FLOWER,
        f_step * rotate_about(glm::vec4(1, 0, 0, 0),  angle/2) * glm::mat4(glm::mat3(0.6)),
        this->flower_mat,
        std::vector<L_node *>{},
        flower_mesh->getSize()
    };
    L_node * flower_2 = new L_node{
        Flower_Component::FLOWER,
        f_step * rotate_about(glm::vec4(1, 0, 0, 0),  -angle/2) * glm::mat4(glm::mat3(0.6)),
        this->flower_mat,
        std::vector<L_node *>{},
        flower_mesh->getSize()
    };
    for(int i = 0; i < 2; ++i){

        L_node * branch = new L_node{
            Flower_Component::STEM,
            f_step * rotate_about(glm::vec4(1, 0, 0, 0),  angle - 2 * i * angle + M_PI) * glm::mat4(glm::mat3(0.6)),
            this->stem_mat,
            std::vector<L_node *>{i%2 == 0? flower_2 : flower},
            lily_stem->getSize()
        };
        branches.push_back(branch);
    }
    if(number > 0){
        branches.push_back(rule1(number - 1));
    }
    L_node * stem = new L_node{
        Flower_Component::STEM,
        f_step * rotate_about(glm::vec4(0, 1, 0, 0), angle),
        this->stem_mat,
        branches,
        lily_stem->getSize()
    };
    return stem;
}

L_node * Lily::rule2(int number, L_node * next, bool start){
    std::vector<L_node *> branches;
    for(int i = 0; i < 2; ++i){

        L_node * branch = new L_node{
            Flower_Component::LEAF,
            f_step * rotate_about(glm::vec4(0, 1, 0, 0),  M_PI * i) * glm::mat4(glm::mat3(0.36)),
            this->leaf_mat,
            std::vector<L_node *>{},
            leaf_mesh->getSize()
        };
        branches.push_back(branch);
    }
    if(number > 0){
        branches.push_back(rule2(number - 1, next, false));
    }
    else{
        branches.push_back(next);
    }
    L_node * stem = new L_node{
        Flower_Component::STEM,
        (start? glm::mat4(1.0) : f_step)* rotate_about(glm::vec4(0, 1, 0, 0), angle),
        this->stem_mat,
        branches,
        lily_stem->getSize()
    };
    return stem;
}
