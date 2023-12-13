#include "realtime.h"

#include <QCoreApplication>
#include <QMouseEvent>
#include <QKeyEvent>
#include <iostream>
#include "settings.h"
#include "skybox/skybox_utils.h"
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
    setupSkybox();
    setupFlowers();
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

    glActiveTexture(m_terrain_rocks_texture_slot);
    glBindTexture(GL_TEXTURE_2D, m_terrain_rocks_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, rocks_texture.width(), rocks_texture.height(), 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, rocks_texture.bits());

    // interpolation modes
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    glActiveTexture(m_terrain_sandgrass_texture_slot);
    glBindTexture(GL_TEXTURE_2D, m_terrain_sandgrass_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, sandgrass_texture.width(), sandgrass_texture.height(), 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, sandgrass_texture.bits());

    // interpolation modes
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    GLint rocks_texture_location = glGetUniformLocation(m_terrain_shader, "rocks_texture");
    GLint sandgrass_texture_location = glGetUniformLocation(m_terrain_shader, "sandgrass_texture");
    glUniform1i(rocks_texture_location, m_terrain_rocks_texture_slot_number);
    glUniform1i(sandgrass_texture_location, m_terrain_sandgrass_texture_slot_number);
    glUseProgram(0);
}

void Realtime::setupSkybox() {
    m_skybox_shader = ShaderLoader::createShaderProgram(":/resources/shaders/skybox.vert",
                                                        ":/resources/shaders/skybox.frag");

    glUseProgram(m_skybox_shader);
    GLint skyboxLocation = glGetUniformLocation(m_skybox_shader, "skybox");
    glUniform1i(skyboxLocation, m_skybox_cube_map_texture_slot_number);
    glUseProgram(0);

    glGenVertexArrays(1, &m_skybox_vao);
    glGenBuffers(1, &m_skybox_vbo);
    glGenBuffers(1, &m_skybox_ebo);

    glBindVertexArray(m_skybox_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_skybox_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_skybox_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(skyboxIndices), &skyboxIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    setupSkyboxTextures();
}

void Realtime::setupSkyboxTextures() {
    std::string facesCubemap[6] = {
        ":/resources/images/right.jpg",
        ":/resources/images/left.jpg",
        ":/resources/images/top.jpg",
        ":/resources/images/bottom.jpg",
        ":/resources/images/front.jpg",
        ":/resources/images/back.jpg"
    };

    glGenTextures(1, &m_skybox_cube_map_texture);
    glActiveTexture(m_skybox_cube_map_texture_slot);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_skybox_cube_map_texture);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS); // optional

    for (unsigned int i = 0; i < 6; i++) {
        QImage face_texture = QImage(facesCubemap[i].c_str());
        if (face_texture.isNull()) {
            std::cerr << "Failed to load texture" << std::endl;
        }

        // do not mirror face texture, don't fucking ask me why this works
        face_texture = face_texture.convertToFormat(QImage::Format_RGBA8888);

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, face_texture.width(), face_texture.height(), 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, face_texture.bits());
    }
    initialized = true;
}

void Realtime::setupFlowers() {
    m_flower_shader = ShaderLoader::createShaderProgram(":/resources/shaders/flower.vert",
                                                        ":/resources/shaders/flower.frag");

    glUseProgram(m_flower_shader);

    GLint ka_Location = glGetUniformLocation(m_flower_shader, "k_a");
    glUniform1f(ka_Location, 0.5);

    GLint kd_Location = glGetUniformLocation(m_flower_shader, "k_d");
    glUniform1f(kd_Location, 0.7);

    GLint ks_Location = glGetUniformLocation(m_flower_shader, "k_s");
    glUniform1f(ks_Location, 0.54);

    GLint timeofdayObj = glGetUniformLocation(m_flower_shader, "timeofday");
    glUniform1f(timeofdayObj, (float) settings.timeOfDay);;

    SceneLightData light{};
    // TODO REPLACE WITH DIRECTIONAL LIGHT FROM TIMEOFDAY
    light.type = LightType::LIGHT_DIRECTIONAL;
    light.pos = glm::vec4(1.f, 1.f, 1.f, 1.f);
    light.dir = glm::vec4(0.25, -1, -1, 0.f);
    light.color = glm::vec4(0.5f, 0.5f, 0.5f, 1);
    GLint lightsNum_Location = glGetUniformLocation(m_flower_shader, "lightsNum");
    glUniform1i(lightsNum_Location, 1);

    GLint lightPos_Location = glGetUniformLocation(m_flower_shader, "worldLightPos[0]");
    glUniform4f(lightPos_Location, light.pos[0], light.pos[1], light.pos[2], light.pos[3]);

    GLint lightDir_Location = glGetUniformLocation(m_flower_shader, "worldLightDir[0]");
    glUniform4f(lightDir_Location, light.dir[0], light.dir[1], light.dir[2], light.dir[3]);

    GLint lightColor_Location = glGetUniformLocation(m_flower_shader, "worldLightCol[0]");
    glUniform4f(lightColor_Location, light.color[0], light.color[1], light.color[2], light.color[3]);

    GLint lightType_Location = glGetUniformLocation(m_flower_shader, "lightType[0]");
    glUniform1i(lightType_Location, lightTypeToNum(light.type));

    glGenVertexArrays(1, &m_flower_vao);

    glGenBuffers(1, &m_flower_data_vbo);
    glGenBuffers(1, &m_flower_ctms_vbo);
    glGenBuffers(1, &m_flower_i_ctms_vbo);

    glUseProgram(0);
}

void Realtime::drawFlowerComponent(std::vector<float> component_tris, std::vector<glm::mat4> ctms,
                                   std::vector<glm::mat3> i_ctms) {
    glBindVertexArray(m_flower_vao);

    if (ctms.size() != i_ctms.size()) {
        throw std::runtime_error("ctms and i_ctms must be the same size, wtf");
    }

    glBindBuffer(GL_ARRAY_BUFFER, m_flower_data_vbo);
    glBufferData(GL_ARRAY_BUFFER, component_tris.size() * sizeof(GLfloat), component_tris.data(), GL_STATIC_DRAW);

    // vertex position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), nullptr);
    // normal position
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), reinterpret_cast<void *>(3 * sizeof(GLfloat)));
    // uv_crd position
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), reinterpret_cast<void *>(6 * sizeof(GLfloat)));

    glBindBuffer(GL_ARRAY_BUFFER, m_flower_ctms_vbo);
    glBufferData(GL_ARRAY_BUFFER, ctms.size() * 16 * sizeof(float), ctms.data(), GL_DYNAMIC_DRAW);

    int ctms_loc = 3;
    for (int i = 0; i < 4; i++) {
        glEnableVertexAttribArray(ctms_loc + i);
        glVertexAttribPointer(ctms_loc + i, 4, GL_FLOAT, GL_FALSE,
                              16 * sizeof(float), reinterpret_cast<void *>(4 * sizeof(float) * i));
        glVertexAttribDivisor(ctms_loc + i, 1); // instanced vertex attribute
    }

    glBindBuffer(GL_ARRAY_BUFFER, m_flower_i_ctms_vbo);
    glBufferData(GL_ARRAY_BUFFER, i_ctms.size() * 16 * sizeof(float), i_ctms.data(), GL_DYNAMIC_DRAW);
    
    int i_ctms_loc = 7;
    for (int i = 0; i < 3; i++) {
        glEnableVertexAttribArray(i_ctms_loc + i);
        glVertexAttribPointer(i_ctms_loc + i, 3, GL_FLOAT, GL_FALSE,
                              9 * sizeof(float), reinterpret_cast<void *>(3 * sizeof(float) * i));
        glVertexAttribDivisor(i_ctms_loc + i, 1); // instanced vertex attribute
    }

    glDrawArraysInstanced(GL_TRIANGLES, 0, component_tris.size() / 6, (GLsizei) ctms.size());

    glBindVertexArray(0);
}

void Realtime::paintFlowers() {
    glUseProgram(m_flower_shader);

    GLint camPosLocation = glGetUniformLocation(m_flower_shader, "worldCameraPos");
    glm::vec4 camPos = m_camera.getCameraPos();
    glUniform4f(camPosLocation, camPos[0], camPos[1], camPos[2], camPos[3]);

    GLint viewLocation = glGetUniformLocation(m_flower_shader, "viewMatrix");
    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &m_camera.getViewMatrix()[0][0]);

    GLint projectionLocation = glGetUniformLocation(m_flower_shader, "projectionMatrix");
    glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &m_camera.getProjectionMatrix()[0][0]);

    Lily *lily = new Lily();
    FlowerCTMCollection lily_ctms = Flower::collectCTMs(lily->flowerData);

    drawFlowerComponent(Lily::LILY_FLOWER_MESH->getTriangles(), lily_ctms.flowerCTMs, lily_ctms.flowerInverseCTMS);
    drawFlowerComponent(Lily::LILY_STEM_MESH->getTriangles(), lily_ctms.stemCTMs, lily_ctms.stemInverseCTMS);
    drawFlowerComponent(Lily::LILY_LEAF_MESH->getTriangles(), lily_ctms.leafCTMs, lily_ctms.leafInverseCTMS);

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
    glActiveTexture(m_terrain_rocks_texture_slot);
    glBindTexture(GL_TEXTURE_2D, m_terrain_rocks_texture);
    glActiveTexture(m_terrain_sandgrass_texture_slot);
    glBindTexture(GL_TEXTURE_2D, m_terrain_sandgrass_texture);

    glPolygonMode(GL_FRONT_AND_BACK, settings.terrainWireframe ? GL_LINE : GL_FILL);
    glDrawArrays(GL_TRIANGLES, 0, res * res * 6);

    glActiveTexture(m_terrain_rocks_texture_slot);
    glBindTexture(GL_TEXTURE_2D, 0);
    glActiveTexture(m_terrain_sandgrass_texture_slot);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glUseProgram(0);
}

void Realtime::paintSkybox() {
    glDepthFunc(GL_LEQUAL);

    glUseProgram(m_skybox_shader);
    glm::mat4 view = glm::mat4(glm::mat3(m_camera.getViewMatrix()));
    glm::mat4 projection = m_camera.getProjectionMatrix();
    glUniformMatrix4fv(glGetUniformLocation(m_skybox_shader, "view"), 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(m_skybox_shader, "projection"), 1, GL_FALSE, &projection[0][0]);

    GLint timeofdayLocation = glGetUniformLocation(m_skybox_shader, "timeofday");
    glUniform1f(timeofdayLocation, (float) settings.timeOfDay);

    glBindVertexArray(m_skybox_vao);
    glActiveTexture(m_skybox_cube_map_texture_slot);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_skybox_cube_map_texture);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glDepthFunc(GL_LESS);
    glUseProgram(0);
}

void Realtime::paintGL() {
    // Students: anything requiring OpenGL calls every frame should be done here
    paintTerrain();
    paintSkybox();
    paintFlowers();
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