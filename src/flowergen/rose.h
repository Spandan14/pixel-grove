#ifndef ROSE_H
#define ROSE_H



#include "src/flowergen/flowers.h"
#include "../utils/scenedata.h"
class Rose
{
public:
    Rose();

private:
    std::vector<GLuint> m_vbos;
    std::vector<GLuint> m_vaos;

    SceneMaterial flower_mat;
    SceneMaterial leaf_mat;
    float angle;

};

#endif // ROSE_H
