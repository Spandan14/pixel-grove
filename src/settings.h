#ifndef SETTINGS_H
#define SETTINGS_H

#include <string>

struct Settings {
    std::string sceneFilePath;
    int timeOfDay = 0;
    int terrainResolution = 100;
    int shapeParameter2 = 1;
    float scaleX = 1;
    float scaleY = 1;
    int textureStride = 10;
    float terrainAmplification = 1.0f;
    bool terrainWireframe = false;
};


// The global Settings object, will be initialized by MainWindow
extern Settings settings;

#endif // SETTINGS_H
