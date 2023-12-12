#include "realtime.h"

#include <QCoreApplication>
#include <QMouseEvent>
#include <QKeyEvent>
#include <iostream>
#include "settings.h"
#include "src/utils/shaderloader.h"
#include "glm/ext/matrix_transform.hpp"
#include "glm/ext/matrix_clip_space.hpp"

// ================== Project 5: Lights, Camera

Realtime::Realtime(QWidget *parent)
        : QOpenGLWidget(parent)
{
    m_prev_mouse_pos = glm::vec2(size().width()/2, size().height()/2);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);

    m_keyMap[Qt::Key_W]       = false;
    m_keyMap[Qt::Key_A]       = false;
    m_keyMap[Qt::Key_S]       = false;
    m_keyMap[Qt::Key_D]       = false;
    m_keyMap[Qt::Key_Control] = false;
    m_keyMap[Qt::Key_Space]   = false;

    // If you must use this function, do not edit anything above this
}

void Realtime::finish() {
    killTimer(m_timer);
    this->makeCurrent();

    // Students: anything requiring OpenGL calls when the program exits should be done here

    this->doneCurrent();
}

void Realtime::initializeGL() {
    m_devicePixelRatio = this->devicePixelRatio();

    m_timer = startTimer(1000/60);
    m_elapsedTimer.start();

    // Initializing GL.
    // GLEW (GL Extension Wrangler) provides access to OpenGL functions.
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "Error while initializing GL: " << glewGetErrorString(err) << std::endl;
    }
    std::cout << "Initialized GL: Version " << glewGetString(GLEW_VERSION) << std::endl;

    // Allows OpenGL to draw objects appropriately on top of one another
    glEnable(GL_DEPTH_TEST);
    // Tells OpenGL to only draw the front face
    glEnable(GL_CULL_FACE);
    // Tells OpenGL how big the screen is
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);

    initialized = true;

    // Students: anything requiring OpenGL calls when the program starts should be done here
    setupCamera();
    setupTerrain();
}

void Realtime::setupCamera() {
    SceneCameraData initialData{};
    initialData.pos = glm::vec4(1.f, 1.f, 1.f, 1.f);
    initialData.look = glm::vec4(-1.f, -1.f, -1.f, 0.f);
    initialData.up = glm::vec4(0.f, 1.f, 0.f, 0.f);
    initialData.heightAngle = M_PI / 4.f;

    m_camera = Camera(size().width(), size().height(), initialData);
}

void Realtime::setupTerrain() {
    glClearColor(0, 0, 0, 1);

    m_terrain_shader = ShaderLoader::createShaderProgram(":/resources/shaders/terrain.vert",
                                                        ":/resources/shaders/terrain.frag");

    glUseProgram(m_terrain_shader);

    m_proj_matrix_loc = glGetUniformLocation(m_terrain_shader, "projMatrix");
    m_mv_matrix_loc = glGetUniformLocation(m_terrain_shader, "mvMatrix");

    glGenVertexArrays(1, &m_terrain_vao);
    glBindVertexArray(m_terrain_vao);

    std::vector<GLfloat> vertices = m_terrain.generateTerrain();

    glGenBuffers(1, &m_terrain_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_terrain_vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat),
                          nullptr);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat),
                          reinterpret_cast<void *>(3 * sizeof(GLfloat)));

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat),
                          reinterpret_cast<void *>(6 * sizeof(GLfloat)));

    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(GLfloat),
                            reinterpret_cast<void *>(9 * sizeof(GLfloat)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glUseProgram(0);

    setupTerrainTextures();
}

void Realtime::setupTerrainTextures() {
    glUseProgram(m_terrain_shader);

    QImage rocks_texture = QImage(":/resources/textures/terrain-rock1.png");
    if (rocks_texture.isNull()) {
        std::cerr << "Failed to load texture" << std::endl;
    }
    rocks_texture = rocks_texture.convertToFormat(QImage::Format_RGBA8888).mirrored();

    QImage sandgrass_texture = QImage(":/resources/textures/terrain-sandgrass1.png");
    if (sandgrass_texture.isNull()) {
        std::cerr << "Failed to load texture" << std::endl;
    }
    sandgrass_texture = sandgrass_texture.convertToFormat(QImage::Format_RGBA8888).mirrored();

    glGenTextures(1, &m_terrain_rocks_texture);
    glGenTextures(1, &m_terrain_sandgrass_texture);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_terrain_rocks_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, rocks_texture.width(), rocks_texture.height(), 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, rocks_texture.bits());

    // interpolation modes
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_terrain_sandgrass_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, sandgrass_texture.width(), sandgrass_texture.height(), 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, sandgrass_texture.bits());

    // interpolation modes
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    GLint rocks_texture_location = glGetUniformLocation(m_terrain_shader, "rocks_texture");
    GLint sandgrass_texture_location = glGetUniformLocation(m_terrain_shader, "sandgrass_texture");
    glUniform1i(rocks_texture_location, 0);
    glUniform1i(sandgrass_texture_location, 1);
    glUseProgram(0);
}

void Realtime::paintTerrain() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    glUseProgram(m_terrain_shader);

    glUniformMatrix4fv(m_proj_matrix_loc, 1, GL_FALSE, &m_camera.getProjectionMatrix()[0][0]);
    glm::mat4 mv_matrix = m_camera.getViewMatrix();
    glUniformMatrix4fv(m_mv_matrix_loc, 1, GL_FALSE, &mv_matrix[0][0]);
    glUniform1i(glGetUniformLocation(m_terrain_shader, "wireShadeOn"), settings.terrainWireframe);

    float lightAngle = settings.timeOfDay * 2 * M_PI / 24.f;
    glm::vec3 lightDir = glm::normalize(glm::vec3(-1.f, -glm::sin(lightAngle), -glm::cos(lightAngle)));
    glUniform3fv(glGetUniformLocation(m_terrain_shader, "lightDir"), 1, &lightDir[0]);

    glUniform4fv(glGetUniformLocation(m_terrain_shader, "cameraPos"), 1, &m_camera.getCameraPos()[0]);

    int res = m_terrain.getResolution();

    glBindVertexArray(m_terrain_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_terrain_vbo);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_terrain_rocks_texture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_terrain_sandgrass_texture);

    glPolygonMode(GL_FRONT_AND_BACK, settings.terrainWireframe ? GL_LINE : GL_FILL);
    glDrawArrays(GL_TRIANGLES, 0, res * res * 6);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glUseProgram(0);
}

void Realtime::paintGL() {
    // Students: anything requiring OpenGL calls every frame should be done here
    paintTerrain();
}

void Realtime::resizeGL(int w, int h) {
    // Tells OpenGL how big the screen is
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);

    // Students: anything requiring OpenGL calls when the program starts should be done here
}

void Realtime::sceneChanged() {

    update(); // asks for a PaintGL() call to occur
}

void Realtime::settingsChanged() {

    m_terrain.setResolution(settings.terrainResolution);
    m_terrain.m_scale_x = settings.scaleX;
    m_terrain.m_scale_y = settings.scaleY;
    m_terrain.uv_texture_stride = settings.textureStride;
    m_terrain.terrain_amplification = settings.terrainAmplification;
    if (initialized)
        setupTerrain();
    update(); // asks for a PaintGL() call to occur
}

// ================== Project 6: Action!

void Realtime::keyPressEvent(QKeyEvent *event) {
    m_keyMap[Qt::Key(event->key())] = true;
}

void Realtime::keyReleaseEvent(QKeyEvent *event) {
    m_keyMap[Qt::Key(event->key())] = false;
}

void Realtime::mousePressEvent(QMouseEvent *event) {
    if (event->buttons().testFlag(Qt::LeftButton)) {
        m_mouseDown = true;
        m_prev_mouse_pos = glm::vec2(event->position().x(), event->position().y());
    }
}

void Realtime::mouseReleaseEvent(QMouseEvent *event) {
    if (!event->buttons().testFlag(Qt::LeftButton)) {
        m_mouseDown = false;
    }
}

void Realtime::mouseMoveEvent(QMouseEvent *event) {
    if (m_mouseDown) {
        int posX = event->position().x();
        int posY = event->position().y();
        int deltaX = posX - m_prev_mouse_pos.x;
        int deltaY = posY - m_prev_mouse_pos.y;
        m_prev_mouse_pos = glm::vec2(posX, posY);

        // Use deltaX and deltaY here to rotate
        m_camera.updateRotation(deltaX, deltaY);

        update(); // asks for a PaintGL() call to occur
    }
}

void Realtime::timerEvent(QTimerEvent *event) {
    int elapsedms   = m_elapsedTimer.elapsed();
    float deltaTime = elapsedms * 0.001f;
    m_elapsedTimer.restart();

    // Use deltaTime and m_keyMap here to move around
    m_camera.updatePos(m_keyMap, deltaTime * 3.f);

    update(); // asks for a PaintGL() call to occur
}

// DO NOT EDIT
void Realtime::saveViewportImage(std::string filePath) {
    // Make sure we have the right context and everything has been drawn
    makeCurrent();

    int fixedWidth = 1024;
    int fixedHeight = 768;

    // Create Frame Buffer
    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // Create a color attachment texture
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, fixedWidth, fixedHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

    // Optional: Create a depth buffer if your rendering uses depth testing
    GLuint rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, fixedWidth, fixedHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Error: Framebuffer is not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return;
    }

    // Render to the FBO
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glViewport(0, 0, fixedWidth, fixedHeight);

    // Clear and render your scene here
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    paintGL();

    // Read pixels from framebuffer
    std::vector<unsigned char> pixels(fixedWidth * fixedHeight * 3);
    glReadPixels(0, 0, fixedWidth, fixedHeight, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());

    // Unbind the framebuffer to return to default rendering to the screen
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Convert to QImage
    QImage image(pixels.data(), fixedWidth, fixedHeight, QImage::Format_RGB888);
    QImage flippedImage = image.mirrored(); // Flip the image vertically

    // Save to file using Qt
    QString qFilePath = QString::fromStdString(filePath);
    if (!flippedImage.save(qFilePath)) {
        std::cerr << "Failed to save image to " << filePath << std::endl;
    }

    // Clean up
    glDeleteTextures(1, &texture);
    glDeleteRenderbuffers(1, &rbo);
    glDeleteFramebuffers(1, &fbo);
}