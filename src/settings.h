#ifndef SETTINGS_H
#define SETTINGS_H

#include <string>

struct Settings {
    std::string sceneFilePath;
    int timeOfDay = 0;
    int terrainResolution = 100;
    int shapeParameter2 = 1;
    float nearPlane = 1;
    float farPlane = 1;
    bool terrainWireframe = false;
};


// The global Settings object, will be initialized by MainWindow
extern Settings settings;

#endif // SETTINGS_H
