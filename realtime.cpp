#include "realtime.h"
#include <QCoreApplication>
#include <QMouseEvent>
#include <QKeyEvent>
#include <iostream>
#include "settings.h"
#include "sphere.h"
#include "cube.h"
#include "cylinder.h"
#include "cone.h"
#include "sceneparser.h"

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
    glDeleteBuffers(1, &m_cylinder_vbo);
    glDeleteVertexArrays(1, &m_cylinder_vao);
    glDeleteBuffers(1, &m_cone_vbo);
    glDeleteVertexArrays(1, &m_cone_vao);
    glDeleteBuffers(1, &m_cube_vbo);
    glDeleteVertexArrays(1, &m_cube_vao);
    glDeleteBuffers(1, &m_sphere_vbo);
    glDeleteVertexArrays(1, &m_sphere_vao);
    glDeleteProgram(m_shader);
    glDeleteTextures(1, &m_fbo_texture);
    glDeleteRenderbuffers(1, &m_fbo_renderbuffer);
    glDeleteFramebuffers(1, &m_fbo);
    glUseProgram(0);
    this->doneCurrent();
}

void Realtime::initializeGL() {
    m_devicePixelRatio = this->devicePixelRatio();
    m_defaultFBO = 2;
    m_screen_width = size().width() * m_devicePixelRatio;
    m_screen_height = size().height() * m_devicePixelRatio;
    m_fbo_width = m_screen_width;
    m_fbo_height = m_screen_height;

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

    // Students: anything requiring OpenGL calls when the program starts should be done here
    m_shader = ShaderLoader::createShaderProgram(":/resources/shaders/phong.vert", ":/resources/shaders/phong.frag");
    m_texture_shader = ShaderLoader::createShaderProgram(":/resources/shaders/texture.vert", ":/resources/shaders/texture.frag");

    initializeShape(PrimitiveType::PRIMITIVE_SPHERE);
    initializeShape(PrimitiveType::PRIMITIVE_CUBE);
    initializeShape(PrimitiveType::PRIMITIVE_CONE);
    initializeShape(PrimitiveType::PRIMITIVE_CYLINDER);
    initialized = true;

    glUseProgram(m_texture_shader);
    GLint textureLocation = glGetUniformLocation(m_texture_shader, "myTexture");
    glUniform1i(textureLocation, 0);
    glUseProgram(0);

    std::vector<GLfloat> fullscreen_quad_data =
        { //     POSITIONS    //
            // Vertex 1 (top left)
            -1.f,  1.f, 0.0f,
            0.f, 1.f,
            //  Vertex 2 (bottom left)
            -1.f, -1.f, 0.0f,
            0.f, 0.f,
            // Vertex 3 (bottom right)
            1.f, -1.f, 0.0f,
            1.f, 0.f,
            // Vertex 4 (top right)
            1.f,  1.f, 0.0f,
            1.f, 1.f,
            // Vertex 5 (top left)
            -1.f,  1.f, 0.0f,
            0.f, 1.f,
            // Vertex 6 (bottom right)
            1.f, -1.f, 0.0f,
            1.f, 0.f
        };

    glGenBuffers(1, &m_fullscreen_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_fullscreen_vbo);
    glBufferData(GL_ARRAY_BUFFER, fullscreen_quad_data.size()*sizeof(GLfloat), fullscreen_quad_data.data(), GL_STATIC_DRAW);
    glGenVertexArrays(1, &m_fullscreen_vao);
    glBindVertexArray(m_fullscreen_vao);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), reinterpret_cast<void*>(3 * sizeof(GLfloat)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    makeFBO();
}

void Realtime::paintGL() {
    // Students: anything requiring OpenGL calls every frame should be done here
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(m_shader);
    for (int i = 0; i < sceneData.shapes.size(); i++){
        GLint modelLocation;
        GLint inverseCTMLocation;
        GLint ambient_Location;
        GLint diffuse_Location;
        GLint specular_Location;
        passSceneUniforms();
        passLightUniforms();

        m_shininess = sceneData.shapes[i].primitive.material.shininess;
        GLint shininess_Location = glGetUniformLocation(m_shader, "shininess");
        glUniform1f(shininess_Location, m_shininess);

        ambientColor = sceneData.shapes[i].primitive.material.cAmbient;
        ambient_Location = glGetUniformLocation(m_shader, "ambientColor");
        glUniform4f(ambient_Location, ambientColor[0], ambientColor[1], ambientColor[2], ambientColor[3]);

        diffuseColor = sceneData.shapes[i].primitive.material.cDiffuse;
        diffuse_Location = glGetUniformLocation(m_shader, "diffuseColor");
        glUniform4f(diffuse_Location, diffuseColor[0], diffuseColor[1], diffuseColor[2], diffuseColor[3]);

        specularColor = sceneData.shapes[i].primitive.material.cSpecular;
        specular_Location = glGetUniformLocation(m_shader, "specularColor");
        glUniform4f(specular_Location, specularColor[0], specularColor[1], specularColor[2], specularColor[3]);

        switch(sceneData.shapes[i].primitive.type){
        case PrimitiveType::PRIMITIVE_CONE:
            glBindVertexArray(m_cone_vao);
            m_cone_model = sceneData.shapes[i].ctm;
            modelLocation = glGetUniformLocation(m_shader, "modelMatrix");
            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, &m_cone_model[0][0]);

            m_cone_inverseCTM = glm::transpose(glm::inverse(glm::mat3(m_cone_model)));
            inverseCTMLocation = glGetUniformLocation(m_shader, "inverseCTM");
            glUniformMatrix3fv(inverseCTMLocation, 1, GL_FALSE, &m_cone_inverseCTM[0][0]);

            glDrawArrays(GL_TRIANGLES, 0, coneData.size() / 6);
            glBindVertexArray(0);
            break;
        case PrimitiveType::PRIMITIVE_CUBE:
            glBindVertexArray(m_cube_vao);
            m_cube_model = sceneData.shapes[i].ctm;
            modelLocation = glGetUniformLocation(m_shader, "modelMatrix");
            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, &m_cube_model[0][0]);

            m_cube_inverseCTM = glm::transpose(glm::inverse(glm::mat3(m_cube_model)));
            inverseCTMLocation = glGetUniformLocation(m_shader, "inverseCTM");
            glUniformMatrix3fv(inverseCTMLocation, 1, GL_FALSE, &m_cube_inverseCTM[0][0]);

            glDrawArrays(GL_TRIANGLES, 0, cubeData.size() / 6);
            glBindVertexArray(0);
            break;
        case PrimitiveType::PRIMITIVE_SPHERE:
            glBindVertexArray(m_sphere_vao);
            m_sphere_model = sceneData.shapes[i].ctm;
            modelLocation = glGetUniformLocation(m_shader, "modelMatrix");
            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, &m_sphere_model[0][0]);

            m_sphere_inverseCTM = glm::transpose(glm::inverse(glm::mat3(m_sphere_model)));
            inverseCTMLocation = glGetUniformLocation(m_shader, "inverseCTM");
            glUniformMatrix3fv(inverseCTMLocation, 1, GL_FALSE, &m_sphere_inverseCTM[0][0]);

            glDrawArrays(GL_TRIANGLES, 0, sphereData.size() / 6);
            glBindVertexArray(0);
            break;
        case PrimitiveType::PRIMITIVE_CYLINDER:
            glBindVertexArray(m_cylinder_vao);
            m_cylinder_model = sceneData.shapes[i].ctm;
            modelLocation = glGetUniformLocation(m_shader, "modelMatrix");
            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, &m_cylinder_model[0][0]);

            m_cylinder_inverseCTM = glm::transpose(glm::inverse(glm::mat3(m_cylinder_model)));
            inverseCTMLocation = glGetUniformLocation(m_shader, "inverseCTM");
            glUniformMatrix3fv(inverseCTMLocation, 1, GL_FALSE, &m_cylinder_inverseCTM[0][0]);

            glDrawArrays(GL_TRIANGLES, 0, cylinderData.size() / 6);
            glBindVertexArray(0);
            break;
        case PrimitiveType::PRIMITIVE_MESH:
            break;
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, m_defaultFBO);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    paintTexture(m_fbo_texture, settings.perPixelFilter, settings.kernelBasedFilter);
}

int Realtime::lightTypeToNum(LightType light_type){
    switch(light_type){
    case LightType::LIGHT_DIRECTIONAL:
        return 0;
        break;
    case LightType::LIGHT_POINT:
        return 1;
        break;
    case LightType::LIGHT_SPOT:
        return 2;
        break;
    }
}

void Realtime::passSceneUniforms(){
    GLint ka_Location = glGetUniformLocation(m_shader, "k_a");
    glUniform1f(ka_Location, m_ka);

    GLint kd_Location = glGetUniformLocation(m_shader, "k_d");
    glUniform1f(kd_Location, m_kd);

    GLint ks_Location = glGetUniformLocation(m_shader, "k_s");
    glUniform1f(ks_Location, m_ks);

    m_view = cam->getViewMatrix();
    m_proj = cam->getProjectionMatrix();

    GLint camPosLocation = glGetUniformLocation(m_shader, "worldCameraPos");
    glm::vec4 camPos = sceneData.cameraData.pos;
    glUniform4f(camPosLocation, camPos[0], camPos[1], camPos[2], camPos[3]);

    GLint viewLocation = glGetUniformLocation(m_shader, "viewMatrix");
    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &m_view[0][0]);

    GLint projectionLocation = glGetUniformLocation(m_shader, "projectionMatrix");
    glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &m_proj[0][0]);
}

void Realtime::passLightUniforms(){
    GLint lightPos_Location;
    GLint lightDir_Location;
    GLint lightColor_Location;
    GLint lightType_Location;
    GLint lightFunction_Location;
    GLint lightAngle_Location;
    GLint lightPenumbra_Location;
    GLint lightsNum_Location = glGetUniformLocation(m_shader, "lightsNum");
    glUniform1i(lightsNum_Location, (int)sceneData.lights.size());
    for (int j = 0; j < sceneData.lights.size(); j++){
        SceneLightData light = sceneData.lights[j];
        lightPos_Location = glGetUniformLocation(m_shader, ("worldLightPos[" + std::to_string(j) + "]").c_str());
        glUniform4f(lightPos_Location, light.pos[0], light.pos[1], light.pos[2], light.pos[3]);

        lightDir_Location = glGetUniformLocation(m_shader, ("worldLightDir[" + std::to_string(j) + "]").c_str());
        glUniform4f(lightDir_Location, light.dir[0], light.dir[1], light.dir[2], light.dir[3]);

        lightColor_Location = glGetUniformLocation(m_shader, ("worldLightCol[" + std::to_string(j) + "]").c_str());
        glUniform4f(lightColor_Location, light.color[0], light.color[1], light.color[2], light.color[3]);

        lightType_Location = glGetUniformLocation(m_shader, ("lightType[" + std::to_string(j) + "]").c_str());
        glUniform1i(lightType_Location, lightTypeToNum(light.type));

        lightFunction_Location = glGetUniformLocation(m_shader, ("lightFunction[" + std::to_string(j) + "]").c_str());
        glUniform3f(lightFunction_Location, light.function[0], light.function[1], light.function[2]);

        lightAngle_Location = glGetUniformLocation(m_shader, ("lightAngle[" + std::to_string(j) + "]").c_str());
        glUniform1f(lightAngle_Location, light.angle);

        lightPenumbra_Location = glGetUniformLocation(m_shader, ("lightPenumbra[" + std::to_string(j) + "]").c_str());
        glUniform1f(lightPenumbra_Location, light.penumbra);
    }
}

void Realtime::resizeGL(int w, int h) {
    // Tells OpenGL how big the screen is
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);

    // Students: anything requiring OpenGL calls when the program starts should be done here
    glDeleteTextures(1, &m_fbo_texture);
    glDeleteRenderbuffers(1, &m_fbo_renderbuffer);
    glDeleteFramebuffers(1, &m_fbo);

    m_screen_width = size().width() * m_devicePixelRatio;
    m_screen_height = size().height() * m_devicePixelRatio;
    m_fbo_width = m_screen_width;
    m_fbo_height = m_screen_height;

    makeFBO();
}

void Realtime::sceneChanged() {
    if(initialized){
        sceneData = RenderData();
        SceneParser::parse(settings.sceneFilePath, sceneData);
        cam = new Camera(size().width(), size().height(), sceneData.cameraData);
        m_ka = sceneData.globalData.ka;
        m_kd = sceneData.globalData.kd;
        m_ks = sceneData.globalData.ks;

        update(); // asks for a PaintGL() call to occur
    }
}

void Realtime::settingsChanged() {
    if(initialized){
        updateCubeVBO();
        updateConeVBO();
        updateCylinderVBO();
        updateSphereVBO();
        update(); // asks for a PaintGL() call to occur
    }
}

void Realtime::paintTexture(GLuint texture, bool perPixel, bool kernelBased){
    glUseProgram(m_texture_shader);
    GLuint perPixel_Location = glGetUniformLocation(m_texture_shader, "perPixel");
    glUniform1i(perPixel_Location, perPixel);

    GLuint kernelBased_Location = glGetUniformLocation(m_texture_shader, "kernelBased");
    glUniform1i(kernelBased_Location, kernelBased);

    GLuint screen_width_Location = glGetUniformLocation(m_texture_shader, "screen_width");
    glUniform1i(screen_width_Location, m_screen_width);
    GLuint screen_height_Location = glGetUniformLocation(m_texture_shader, "screen_height");
    glUniform1i(screen_height_Location, m_screen_height);

    glBindVertexArray(m_fullscreen_vao);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}

void Realtime::makeFBO(){
    glGenTextures(1, &m_fbo_texture);
    glActiveTexture(GL_TEXTURE0);

    glBindTexture(GL_TEXTURE_2D, m_fbo_texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_fbo_width, m_fbo_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);

    glGenRenderbuffers(1, &m_fbo_renderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_fbo_renderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_fbo_width, m_fbo_height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_fbo_texture, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_fbo_renderbuffer);

    glBindFramebuffer(GL_FRAMEBUFFER, m_defaultFBO);
}

// ---------Initialize Shape Functions (VBO / VAO Binding)---------

void Realtime::initializeShape(PrimitiveType shape){
    Sphere my_sphere;
    Cube my_cube;
    Cone my_cone;
    Cylinder my_cylinder;
    switch(shape){
    case PrimitiveType::PRIMITIVE_SPHERE:
        glGenBuffers(1, &m_sphere_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, m_sphere_vbo);
        my_sphere.updateParams(settings.shapeParameter1, settings.shapeParameter2);
        sphereData = my_sphere.generateShape();
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * sphereData.size(), sphereData.data(),  GL_STATIC_DRAW);
        glGenVertexArrays(1, &m_sphere_vao);
        glBindVertexArray(m_sphere_vao);
        break;
    case PrimitiveType::PRIMITIVE_CUBE:
        glGenBuffers(1, &m_cube_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, m_cube_vbo);
        my_cube.updateParams(settings.shapeParameter1);
        cubeData = my_cube.generateShape();
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * cubeData.size(), cubeData.data(),  GL_STATIC_DRAW);
        glGenVertexArrays(1, &m_cube_vao);
        glBindVertexArray(m_cube_vao);
        break;
    case PrimitiveType::PRIMITIVE_CONE:
        glGenBuffers(1, &m_cone_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, m_cone_vbo);
        my_cone.updateParams(settings.shapeParameter1, settings.shapeParameter2);
        coneData = my_cone.generateShape();
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * coneData.size(), coneData.data(),  GL_STATIC_DRAW);
        glGenVertexArrays(1, &m_cone_vao);
        glBindVertexArray(m_cone_vao);
        break;
    case PrimitiveType::PRIMITIVE_CYLINDER:
        glGenBuffers(1, &m_cylinder_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, m_cylinder_vbo);
        my_cylinder.updateParams(settings.shapeParameter1, settings.shapeParameter2);
        cylinderData = my_cylinder.generateShape();
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * cylinderData.size(), cylinderData.data(),  GL_STATIC_DRAW);
        glGenVertexArrays(1, &m_cylinder_vao);
        glBindVertexArray(m_cylinder_vao);
        break;
    case PrimitiveType::PRIMITIVE_MESH:
        break;
    }
    glEnableVertexAttribArray(0); // 0 = Position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 24, reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(1); // 1 = Normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 24, reinterpret_cast<void*>(3 * sizeof(GLfloat)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Realtime::updateSphereVBO(){
    glBindBuffer(GL_ARRAY_BUFFER, m_sphere_vbo);
    Sphere my_sphere;
    my_sphere.updateParams(settings.shapeParameter1, settings.shapeParameter2);
    sphereData = my_sphere.generateShape();
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * sphereData.size(), sphereData.data(),  GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Realtime::updateCubeVBO(){
    glBindBuffer(GL_ARRAY_BUFFER, m_cube_vbo);
    Cube my_cube;
    my_cube.updateParams(settings.shapeParameter1);
    cubeData = my_cube.generateShape();
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * cubeData.size(), cubeData.data(),  GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Realtime::updateConeVBO(){
    glBindBuffer(GL_ARRAY_BUFFER, m_cone_vbo);
    Cone my_cone;
    my_cone.updateParams(settings.shapeParameter1, settings.shapeParameter2);
    coneData = my_cone.generateShape();
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * coneData.size(), coneData.data(),  GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Realtime::updateCylinderVBO(){
    glBindBuffer(GL_ARRAY_BUFFER, m_cylinder_vbo);
    Cylinder my_cylinder;
    my_cylinder.updateParams(settings.shapeParameter1, settings.shapeParameter2);
    cylinderData = my_cylinder.generateShape();
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * cylinderData.size(), cylinderData.data(),  GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
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
    float speed = 5.f * deltaTime;
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
