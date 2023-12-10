#ifndef TULIP_H
#define TULIP_H


#include "src/flowergen/flowers.h"
class tulip : public Flower
{
public:
    tulip();
private:
    std::vector<GLuint> m_vaos;
    std::vector<GLuint> m_vbos;
    std::vector<GLint> m_flower;
    std::vector<GLint> m_leaf;
};

#endif // TULIP_H
