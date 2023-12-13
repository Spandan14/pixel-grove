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
#include "src/terrain/terrain.h"
#include "src/camera/camera.h"
#include "src/flowergen/lily.h"
#include "src/flowergen/stem.h"

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

    // initialization
    bool initialized = false;


    // Terrain variables
    void setupTerrain();
    void setupTerrainTextures();
    void paintTerrain();

    GLuint m_terrain_shader;
    GLuint m_terrain_vao;
    GLuint m_terrain_vbo;
    TerrainGenerator m_terrain;

    GLuint m_terrain_rocks_texture;
    GLenum m_terrain_rocks_texture_slot = GL_TEXTURE0;
    int m_terrain_rocks_texture_slot_number = 0;
    GLuint m_terrain_sandgrass_texture;
    GLenum m_terrain_sandgrass_texture_slot = GL_TEXTURE1;
    int m_terrain_sandgrass_texture_slot_number = 1;


    // Skybox variables
    void setupSkybox();
    void setupSkyboxTextures();
    void paintSkybox();

    GLuint m_skybox_shader;
    GLuint m_skybox_vao;
    GLuint m_skybox_vbo;
    GLuint m_skybox_ebo;

    GLuint m_skybox_cube_map_texture;
    GLenum m_skybox_cube_map_texture_slot = GL_TEXTURE2;
    int m_skybox_cube_map_texture_slot_number = 2;


    // Flower variables
    void setupFlowers();
    void drawFlowerComponent(std::vector<float> component_tris, std::vector<glm::mat4> ctms, std::vector<glm::mat3> i_ctms);
    void paintFlowers();

    GLuint m_flower_shader;
    GLuint m_flower_vao;

    GLuint m_flower_data_vbo;
    GLuint m_flower_ctms_vbo;
    GLuint m_flower_i_ctms_vbo;



    // Camera
    Camera m_camera = Camera(0, 0, SceneCameraData());
    void setupCamera();

    int m_proj_matrix_loc = 0;
    int m_mv_matrix_loc = 0;
};