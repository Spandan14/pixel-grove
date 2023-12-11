#version 330 core
layout(location = 0) in vec3 objPosition;
layout(location = 1) in vec3 objNormal;


out vec3 worldPosition;
out vec3 worldNormal;

uniform mat4 modelMatrix;
uniform mat3 inverseCTM;

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main() {
    worldPosition = vec3(modelMatrix * vec4(objPosition, 1.f));
    worldNormal = inverseCTM * objNormal;

    // Recall that transforming normals requires obtaining the inverse-transpose of the model matrix!
    // In projects 5 and 6, consider the performance implications of performing this here.

    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(objPosition, 1.f);
}
