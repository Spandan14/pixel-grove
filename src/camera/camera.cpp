#include <stdexcept>
#include "camera.h"
#include "src/settings.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform2.hpp>

Camera::Camera(int width, int height, const SceneCameraData &camData){
    this->camera_width = width;
    this->camera_height = height;
    this->sc_data = camData;
    this->camera_look = camData.look;
    this->camera_pos = camData.pos;
    this->camera_up = camData.up;
}

void Camera::updatePos(std::unordered_map<Qt::Key, bool> m_keyMap, float speed){
    if(m_keyMap[Qt::Key_W]){
        camera_pos += speed * glm::normalize(camera_look);
    }
    if (m_keyMap[Qt::Key_S]) {
        camera_pos -= speed * glm::normalize(camera_look);
    }
    if (m_keyMap[Qt::Key_A]) {
        glm::vec3 camLook = glm::normalize(glm::vec3(camera_look[0], camera_look[1], camera_look[2]));
        glm::vec3 camUp = glm::normalize(glm::vec3(camera_up[0], camera_up[1], camera_up[2]));
        camera_pos -= speed * glm::vec4(glm::cross(camLook, camUp), 0.f);
    }
    if (m_keyMap[Qt::Key_D]) {
        glm::vec3 camLook = glm::normalize(glm::vec3(camera_look[0], camera_look[1], camera_look[2]));
        glm::vec3 camUp = glm::normalize(glm::vec3(camera_up[0], camera_up[1], camera_up[2]));
        camera_pos += speed * glm::vec4(glm::cross(camLook, camUp), 0.f);
    }
    if (m_keyMap[Qt::Key_Space]) {
        camera_pos += speed * glm::vec4(0.f, 1.f, 0.f, 0.f);
    }
    if (m_keyMap[Qt::Key_Control]) {
        camera_pos += speed * glm::vec4(0.f, -1.f, 0.f, 0.f);
    }
}

glm::vec4 Camera::getCameraPos() {
    return camera_pos;
}

void Camera::updateRotation(int deltaX, int deltaY){
    float sensitivity = 0.003f;

    glm::vec3 camLook = glm::vec3(camera_look[0], camera_look[1], camera_look[2]);
    glm::vec3 camUp = glm::vec3(camera_up[0], camera_up[1], camera_up[2]);
    glm::vec3 right = glm::normalize(glm::cross(camLook, camUp));
    float angleX = sensitivity * deltaX;
    float angleY = sensitivity * deltaY;

    // Horizontal rotation (yaw)
    glm::mat4 yaw = glm::mat4(cos(angleX) + pow(camUp[0], 2) * (1.f - cos(angleX)),
                              camUp[0] * camUp[1] * (1.f - cos(angleX)) - camUp[2] * sin(angleX),
                              camUp[0] * camUp[2] * (1.f - cos(angleX)) + camUp[1] * sin(angleX),
                              0.f,
                              camUp[0] * camUp[1] * (1.f - cos(angleX)) + camUp[2] * sin(angleX),
                              cos(angleX) + pow(camUp[1], 2) * (1.f - cos(angleX)),
                              camUp[1] * camUp[2] * (1.f - cos(angleX)) - camUp[0] * sin(angleX),
                              0.f,
                              camUp[0] * camUp[2] * (1.f - cos(angleX)) - camUp[1] * sin(angleX),
                              camUp[1] * camUp[2] * (1.f - cos(angleX)) + camUp[0] * sin(angleX),
                              cos(angleX) + pow(camUp[2], 2) * (1.f - cos(angleX)), 0.f,
                              0.f, 0.f, 0.f, 1.f);

    camera_look = glm::vec4(glm::mat3(yaw) * glm::vec3(camera_look), 0.f);

    // Vertical rotation (pitch)
    glm::mat4 pitch = glm::mat4(cos(angleY) + pow(right[0], 2) * (1.f - cos(angleY)),
                                right[0] * right[1] * (1.f - cos(angleY)) - right[2] * sin(angleY),
                                right[0] * right[2] * (1.f - cos(angleY)) + right[1] * sin(angleY),
                                0.f,
                                right[0] * right[1] * (1.f - cos(angleY)) + right[2] * sin(angleY),
                                cos(angleY) + pow(right[1], 2) * (1.f - cos(angleY)),
                                right[1] * right[2] * (1.f - cos(angleY)) - right[0] * sin(angleY),
                                0.f,
                                right[0] * right[2] * (1.f - cos(angleY)) - right[1] * sin(angleY),
                                right[1] * right[2] * (1.f - cos(angleY)) + right[0] * sin(angleY),
                                cos(angleY) + pow(right[2], 2) * (1.f - cos(angleY)), 0.f,
                                0.f, 0.f, 0.f, 1.f);
    camera_look = glm::vec4(glm::mat3(pitch) * glm::vec3(camera_look), 0.f);

    camera_look = glm::normalize(camera_look);
}

glm::mat4 Camera::getViewMatrix() const {
    glm::vec3 w = -glm::normalize(camera_look);
    glm::vec3 up = camera_up;
    glm::vec3 v = glm::normalize(up - (glm::dot(up, w) * w));
    glm::vec3 u = glm::normalize(glm::cross(v, w));

    glm::mat4 m_rotate = glm::mat4(u[0], v[0], w[0], 0.f,
                                   u[1], v[1], w[1], 0.f,
                                   u[2], v[2], w[2], 0.f,
                                   0.f, 0.f, 0.f, 1.f);

    glm::mat4 m_translate = glm::mat4(1.f, 0.f, 0.f, 0.f,
                                      0.f, 1.f, 0.f, 0.f,
                                      0.f, 0.f, 1.f, 0.f,
                                      -this->camera_pos[0], -this->camera_pos[1], -this->camera_pos[2], 1.f);

    return m_rotate * m_translate;
}

glm::mat4 Camera::getProjectionMatrix() const{
    return getOpenGLConvertMatrix() * getUnhingingMatrix() * getScalingMatrix();
}

glm::mat4 Camera::getScalingMatrix() const{
    float far = 100.f;
    float widthAngle = 2 * atan(tan(getHeightAngle() / 2) * getAspectRatio());
    return glm::mat4(1 / (far * tan(widthAngle / 2.f)), 0.f, 0.f, 0.f,
                     0.f, 1 / (far * glm::tan(getHeightAngle() / 2.f)), 0.f , 0.f,
                     0.f, 0.f, 1 / far, 0.f,
                     0.f, 0.f, 0.f, 1.f);
}

glm::mat4 Camera::getUnhingingMatrix() const{
    float c = -0.001f; // OVERRIDE for now
    return glm::mat4(1.f, 0.f, 0.f, 0.f,
                     0.f, 1.f, 0.f, 0.f,
                     0.f, 0.f, 1.f / (1 + c), -1.f,
                     0.f, 0.f, -c / (1 + c), 0.f);
}

glm::mat4 Camera::getOpenGLConvertMatrix() const{
    return glm::mat4(1.f, 0.f, 0.f, 0.f,
                     0.f, 1.f, 0.f, 0.f,
                     0.f, 0.f, -2.f, 0.f,
                     0.f, 0.f, -1.f, 1.f);
}

glm::mat4 Camera::getInverseViewMatrix() const {
    return glm::inverse(getViewMatrix());
}

float Camera::getAspectRatio() const {
    return ((float)camera_width / camera_height);
}

float Camera::getHeightAngle() const {
    return this->sc_data.heightAngle;
}

glm::vec4 Camera::getWorldSpaceLook() const {
    return camera_look;
}