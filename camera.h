#pragma once

#include <glm/glm.hpp>
#include "scenedata.h"
#include <QCoreApplication>
#include <QMouseEvent>
#include <QKeyEvent>

class Camera {
public:
    Camera(int width, int height, const SceneCameraData &camData);

    void updatePos(std::unordered_map<Qt::Key, bool> m_keyMap, float speed);
    void updateRotation(int deltaX, int deltaY);
    glm::vec4 getCameraPos();
    glm::mat4 getTranslationMatrix(glm::vec3 vector) const;

    glm::mat4 getViewMatrix() const;

    glm::mat4 getInverseViewMatrix() const;

    // Returns the aspect ratio of the camera.
    float getAspectRatio() const;

    // Returns the height angle of the camera in RADIANS.
    float getHeightAngle() const;

    float getFocalLength() const;

    float getAperture() const;
    glm::mat4 getProjectionMatrix() const;
    glm::mat4 getScalingMatrix() const;
    glm::mat4 getUnhingingMatrix() const;
    glm::mat4 getOpenGLConvertMatrix() const;

private:
    int camera_width;
    int camera_height;
    SceneCameraData sc_data;
    glm::vec4 camera_look;
    glm::vec4 camera_pos;
    glm::vec4 camera_up;
};
