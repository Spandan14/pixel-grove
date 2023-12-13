#include <glm/glm.hpp>

glm::mat4 rotate_about(glm::vec4 axis, float theta) {
    glm::mat4 rotate = glm::mat4(
            glm::cos(theta) + pow(axis[0], 2) * (1 - glm::cos(theta)),
            axis[1] * axis[0] * (1 - glm::cos(theta)) + axis[2] * glm::sin(theta),
            axis[0] * axis[2] * (1 - glm::cos(theta)) - axis[1] * glm::sin(theta), 0,
            axis[0] * axis[1] * (1 - glm::cos(theta)) - axis[2] * glm::sin(theta),
            glm::cos(theta) + pow(axis[1], 2) * (1 - glm::cos(theta)),
            axis[1] * axis[2] * (1 - glm::cos(theta)) + axis[0] * glm::sin(theta), 0,
            axis[0] * axis[2] * (1 - glm::cos(theta)) + axis[1] * glm::sin(theta),
            axis[1] * axis[2] * (1 - glm::cos(theta)) - axis[0] * glm::sin(theta),
            glm::cos(theta) + pow(axis[2], 2) * (1 - glm::cos(theta)), 0,
            0, 0, 0, 1);
    return rotate;
}