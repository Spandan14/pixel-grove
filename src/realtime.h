#pragma once

// Defined before including GLEW to suppress deprecation messages on macOS
#include "src/flowergen/lily.h"
#include "src/flowergen/rose.h"
#include "src/utils/stem.h"
#include "src/flowergen/tulip.h"
#include "src/utils/scenedata.h"
#include "src/utils/sceneparser.h"
#include "src/camera/camera.h"
#include "src/utils/emissive_s.h"

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif
#include <GL/glew.h>
#include <glm/glm.hpp>

#include <unordered_map>
#include <QElapsedTimer>
#include <QOpenGLWidget>
#include <QTime>
#include <QTimer>

class Realtime : public QOpenGLWidget
{
public:
    Realtime(QWidget *parent = nullptr);
    void finish();                                      // Called on program exit
    void sceneChanged();
    void settingsChanged();
    void saveViewportImage(std::string filePath);

public slots:
    void tick(QTimerEvent* event);                      // Called once per tick of m_timer

protected:
    void initializeGL() override;                       // Called once at the start of the program
    void paintGL() override;                            // Called whenever the OpenGL context changes or by an update() request
    void resizeGL(int width, int height) override;      // Called when window size changes

private:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void timerEvent(QTimerEvent *event) override;

    // Tick Related Variables
    int m_timer;                                        // Stores timer which attempts to run ~60 times per second
    QElapsedTimer m_elapsedTimer;                       // Stores timer which keeps track of actual time between frames

    // Input Related Variables
    bool m_mouseDown = false;                           // Stores state of left mouse button
    glm::vec2 m_prev_mouse_pos;                         // Stores mouse position
    std::unordered_map<Qt::Key, bool> m_keyMap;         // Stores whether keys are pressed or not

    // Device Correction Variables
    int m_devicePixelRatio;

    RenderData sceneData;
    Camera* cam;
    GLuint m_shader;
    GLuint m_texture_shader;
    GLuint m_skyblock_shader;

    std::vector<SceneLightData> lights;
    glm::mat4 m_view;
    glm::mat4 m_proj;
    glm::vec4 m_lightPos;

    glm::vec4 ambientColor;
    glm::vec4 diffuseColor;
    glm::vec4 specularColor;
    bool initialized = false;

    GLuint m_fullscreen_vbo;
    GLuint m_fullscreen_vao;

    GLuint skyboxVAO;
    GLuint skyboxVBO;
    GLuint cubemapTexture;
    unsigned int loadCubemap(std::vector<std::string> faces);
    Lily *lily;
    Tulip *tulip;
    Rose *rose;
    Stem * stem;
    std::vector<Flower *> flowerTypes;

    GLuint m_sphere_vao;
    GLuint m_sphere_vbo;
    std::vector<GLfloat> sphereData;
    Emissive_S * sphere;

    int lightTypeToNum(LightType light_type);

    void paintTexture(GLuint texture, bool perPixel, bool kernelBased);
    void makeFBO();
    int m_screen_width;
    int m_screen_height;
    int m_fbo_width;
    int m_fbo_height;
    GLuint m_fbo_texture;
    GLuint m_fbo_renderbuffer;
    GLuint m_fbo;
    GLuint m_defaultFBO;
};
