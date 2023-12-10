#ifndef ROSE_H
#define ROSE_H



#include "src/flowergen/flowers.h"
class Rose
{
public:
    Rose();

private:
    std::vector<GLuint> m_vbos;
    std::vector<GLuint> m_vaos;

    float angle;

};

#endif // ROSE_H
