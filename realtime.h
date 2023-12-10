#pragma once

// Defined before including GLEW to suppress deprecation messages on macOS
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
#include "sceneparser.h"
#include "camera.h"
#include "shaderloader.h"

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

    //OpenGL Variables
    GLuint m_sphere_vbo;
    GLuint m_sphere_vao;
    GLuint m_cube_vbo;
    GLuint m_cube_vao;
    GLuint m_cone_vbo;
    GLuint m_cone_vao;
    GLuint m_cylinder_vbo;
    GLuint m_cylinder_vao;

    void initializeShape(PrimitiveType shape);
    void initializeSphere();
    void updateSphereVBO();
    void initializeCube();
    void updateCubeVBO();
    void initializeCone();
    void updateConeVBO();
    void initializeCylinder();
    void updateCylinderVBO();
    RenderData sceneData;
    Camera* cam;
    GLuint m_shader;
    GLuint m_texture_shader;
    glm::mat4 m_cylinder_model;
    glm::mat3 m_cylinder_inverseCTM;
    glm::mat4 m_cone_model;
    glm::mat3 m_cone_inverseCTM;
    glm::mat4 m_cube_model;
    glm::mat3 m_cube_inverseCTM;
    glm::mat4 m_sphere_model;
    glm::mat3 m_sphere_inverseCTM;
    glm::mat4 m_view;
    glm::mat4 m_proj;
    float m_ka;
    float m_kd;
    float m_ks;
    float m_shininess;
    glm::vec4 m_lightPos;

    std::vector<GLfloat> sphereData;
    std::vector<GLfloat> cubeData;
    std::vector<GLfloat> cylinderData;
    std::vector<GLfloat> coneData;
    glm::vec4 ambientColor;
    glm::vec4 diffuseColor;
    glm::vec4 specularColor;
    bool initialized = false;

    int lightTypeToNum(LightType light_type);
    void passLightUniforms();
    void passSceneUniforms();

    void makeFBO();
    void paintTexture(GLuint texture, bool perPixel, bool kernelBased);
    GLuint m_fbo_texture;
    GLuint m_fbo_renderbuffer;
    GLuint m_fbo;
    GLuint m_defaultFBO;
    int m_fbo_width;
    int m_fbo_height;
    int m_screen_width;
    int m_screen_height;

    GLuint m_fullscreen_vbo;
    GLuint m_fullscreen_vao;

};
