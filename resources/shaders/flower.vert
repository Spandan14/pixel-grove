#version 330 core
layout (location = 0) in vec3 flowerPartPosition;
layout (location = 1) in vec3 flowerPartNormal;
layout (location = 2) in vec2 flowerPartUVCoord;
layout (location = 3) in mat4 flowerPartCTM; // 3/4/5/6
layout (location = 7) in mat3 inverseFlowerPartCTM; // 7/8/9

layout (location = 10) in vec4 flowerPartAmbientColor;
layout (location = 11) in vec4 flowerPartDiffuseColor;
layout (location = 12) in vec4 flowerPartSpecularColor;
layout (location = 13) in vec4 flowerPartEmissiveColor;
layout (location = 14) in float flowerPartShininess;

out vec3 worldPosition;
out vec3 worldNormal;

out vec4 ambientColor;
out vec4 diffuseColor;
out vec4 specularColor;
out vec4 emissiveColor;
out float shininess;

out vec2 uvCoord;

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main() {
    worldPosition = vec3(flowerPartCTM * vec4(flowerPartPosition, 1.f));
    worldNormal = inverseFlowerPartCTM * flowerPartNormal;

    // Recall that transforming normals requires obtaining the inverse-transpose of the model matrix!
    // In projects 5 and 6, consider the performance implications of performing this here.

    gl_Position = projectionMatrix * viewMatrix * flowerPartCTM * vec4(flowerPartPosition, 1.f);

    ambientColor = flowerPartAmbientColor;
    diffuseColor = flowerPartDiffuseColor;
    specularColor = flowerPartSpecularColor;
    emissiveColor = flowerPartEmissiveColor;
    shininess = flowerPartShininess;

    uvCoord = flowerPartUVCoord;
}
