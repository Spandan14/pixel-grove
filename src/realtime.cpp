#include "realtime.h"
#include <QCoreApplication>
#include <QMouseEvent>
#include <QKeyEvent>

#include <iostream>
#include "src/utils/shaderloader.h"
#include "src/flowergen/tulip.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "settings.h"

float skyboxVertices[] =
    {
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f,  1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f
};

unsigned int skyboxIndices[] =
    {
        // Right
        6, 2, 1,
        1, 5, 6,
        // Left
        7, 4, 0,
        0, 3, 7,
        // Top
        6, 5, 4,
        4, 7, 6,
        // Bottom
        2, 3, 0,
        0, 1, 2,
        // Back
        5, 1, 0,
        0, 4, 5,
        // Front
        6, 7, 3,
        3, 2, 6
};

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
}

void Realtime::finish() {
    killTimer(m_timer);
    this->makeCurrent();

    // Students: anything requiring OpenGL calls when the program exits should be done here
    this->tulip->freeMeshes();
    glDeleteProgram(m_skyblock_shader);
    glUseProgram(0);
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

    // Students: anything requiring OpenGL calls when the program starts should be done here
    m_shader = ShaderLoader::createShaderProgram("resources/shaders/phong.vert", "resources/shaders/phong.frag");
    m_skyblock_shader = ShaderLoader::createShaderProgram("resources/shaders/skyblock.vert",
                                                          "resources/shaders/skyblock.frag");

    // Allows OpenGL to draw objects appropriately on top of one another
    glEnable(GL_DEPTH_TEST);
    // Tells OpenGL to only draw the front face
    glEnable(GL_CULL_FACE);
    // Keeps front faces
    glCullFace(GL_BACK);
    // Uses counter clock-wise standard
    glFrontFace(GL_CCW);
    // Tells OpenGL how big the screen is
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);

    this->tulip = new Tulip();
    this->lily = new Lily();
    this->rose = new Rose();

    this->flowerTypes = {tulip, lily, rose};

    SceneCameraData cameraData;
    cameraData.look = glm::vec4(0.f, 0.f, -1.f, 0.f);
    cameraData.pos = glm::vec4(0.f, 0.f, 2.f, 1.f);
    cameraData.up = glm::vec4(0.f, 1.f, 0.f, 0.f);
    cameraData.heightAngle = 45.f;
    cameraData.aperture = 0.f;
    cameraData.focalLength = 30.f;
    cam = new Camera(size().width(), size().height(), cameraData);

    glUseProgram(m_skyblock_shader);
    GLint skyblockLocation = glGetUniformLocation(m_skyblock_shader, "skybox");
    glUniform1i(skyblockLocation, 0);
    glUseProgram(0);

    // Create VAO, VBO, and EBO for the skybox
    unsigned int skyboxEBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glGenBuffers(1, &skyboxEBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyboxEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(skyboxIndices), &skyboxIndices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    std::string facesCubemap[6] =
        {
            "resources/images/right.jpg",
            "resources/images/left.jpg",
            "resources/images/top.jpg",
            "resources/images/bottom.jpg",
            "resources/images/front.jpg",
            "resources/images/back.jpg"
        };

    glGenTextures(1, &cubemapTexture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS); //optional

    for (unsigned int i = 0; i < 6; i++)
    {
        int width, height, nrChannels;
        unsigned char* data = stbi_load(facesCubemap[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            stbi_set_flip_vertically_on_load(false);
            glTexImage2D
                (
                    GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                    0,
                    GL_RGB,
                    width,
                    height,
                    0,
                    GL_RGB,
                    GL_UNSIGNED_BYTE,
                    data
                    );
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Failed to load texture: " << facesCubemap[i] << std::endl;
            stbi_image_free(data);
        }
    }
    initialized = true;
}

void Realtime::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(m_shader);

    GLint ka_Location = glGetUniformLocation(m_shader, "k_a");
    glUniform1f(ka_Location, 0.5);

    GLint kd_Location = glGetUniformLocation(m_shader, "k_d");
    glUniform1f(kd_Location, 0.7);

    GLint ks_Location = glGetUniformLocation(m_shader, "k_s");
    glUniform1f(ks_Location, 0.54);

    GLint timeofdayObj = glGetUniformLocation(m_shader, "timeofday");
    glUniform1f(timeofdayObj, settings.timeOfDay);

    GLint camPosLocation = glGetUniformLocation(m_shader, "worldCameraPos");
    glm::vec4 camPos = cam->getCameraPos();
    glUniform4f(camPosLocation, camPos[0], camPos[1], camPos[2], camPos[3]);

    GLint viewLocation = glGetUniformLocation(m_shader, "viewMatrix");
    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &cam->getViewMatrix()[0][0]);

    GLint projectionLocation = glGetUniformLocation(m_shader, "projectionMatrix");
    glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &cam->getProjectionMatrix()[0][0]);

    SceneLightData light;
    light.type = LightType::LIGHT_DIRECTIONAL;
    light.pos = glm::vec4(1.f, 1.f, 1.f, 1.f);
    light.dir = glm::vec4(0.25, -1, -1, 0.f);
    light.color = glm::vec4(0.5f, 0.5f, 0.5f, 1);
    GLint lightsNum_Location = glGetUniformLocation(m_shader, "lightsNum");
    glUniform1i(lightsNum_Location, 1);

    GLint lightPos_Location = glGetUniformLocation(m_shader, "worldLightPos[0]");
    glUniform4f(lightPos_Location, light.pos[0], light.pos[1], light.pos[2], light.pos[3]);

    GLint lightDir_Location = glGetUniformLocation(m_shader, "worldLightDir[0]");
    glUniform4f(lightDir_Location, light.dir[0], light.dir[1], light.dir[2], light.dir[3]);

    GLint lightColor_Location = glGetUniformLocation(m_shader, "worldLightCol[0]");
    glUniform4f(lightColor_Location, light.color[0], light.color[1], light.color[2], light.color[3]);

    GLint lightType_Location = glGetUniformLocation(m_shader, "lightType[0]");
    glUniform1i(lightType_Location, lightTypeToNum(light.type));
    for(int i = 0; i < 100; ++i){
        for(int j = 0; j<10; ++j){
            glm::vec3 newloc = glm::vec3(i * 0.2, 0, j * 0.2);
            this->lily->drawLilies(m_shader, 0, newloc);
        }
    }



    glUseProgram(0);

    glDepthFunc(GL_LEQUAL);

    glUseProgram(m_skyblock_shader);
    glm::mat4 view = glm::mat4(glm::mat3(cam->getViewMatrix()));
    glm::mat4 projection = cam->getProjectionMatrix();
    glUniformMatrix4fv(glGetUniformLocation(m_skyblock_shader, "view"), 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(m_skyblock_shader, "projection"), 1, GL_FALSE, &projection[0][0]);

    GLint timeofdayLocation = glGetUniformLocation(m_skyblock_shader, "timeofday");
    glUniform1f(timeofdayLocation, settings.timeOfDay);

    glBindVertexArray(skyboxVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glDepthFunc(GL_LESS);
    glUseProgram(0);
}

int Realtime::lightTypeToNum(LightType light_type){
    int return_num;
    switch(light_type){
    case LightType::LIGHT_DIRECTIONAL:
        return_num = 0;
        break;
    case LightType::LIGHT_POINT:
        return_num = 1;
        break;
    case LightType::LIGHT_SPOT:
        return_num = 2;
        break;
    }
    return return_num;
}

void Realtime::resizeGL(int w, int h) {
    // Tells OpenGL how big the screen is

    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);

    // Students: anything requiring OpenGL calls when the program starts should be done here
}

void Realtime::settingsChanged() {
    if (initialized) {
        update(); // asks for a PaintGL() call to occur
    }
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
        cam->updateRotation(deltaX, deltaY);

        update(); // asks for a PaintGL() call to occur
    }
}

void Realtime::timerEvent(QTimerEvent *event) {
    int elapsedms   = m_elapsedTimer.elapsed();
    float deltaTime = elapsedms * 0.001f;
    m_elapsedTimer.restart();

    // Use deltaTime and m_keyMap here to move around
    float speed = 1.f * deltaTime;
    cam->updatePos(m_keyMap, speed);

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
