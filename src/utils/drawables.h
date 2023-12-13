#ifndef DRAWABLES_H
#define DRAWABLES_H

class Drawables {
    virtual void drawMesh() = 0;
    virtual void freeMesh() = 0;
    virtual int getSize() = 0;
};

#endif // DRAWABLES_H
