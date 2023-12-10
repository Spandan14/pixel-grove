#version 330 core

in vec3 worldPosition;
in vec3 worldNormal;

out vec4 fragColor;

uniform float k_a;
uniform vec4 ambientColor;
uniform vec4 diffuseColor;
uniform vec4 specularColor;

uniform float k_d;

uniform int lightType[8];
uniform vec4 worldLightPos[8];
uniform vec4 worldLightDir[8];
uniform vec4 worldLightCol[8];
uniform vec3 lightFunction[8];
uniform int lightsNum;
uniform float lightAngle[8];
uniform float lightPenumbra[8];

uniform float k_s;
uniform float shininess;
uniform vec4 worldCameraPos;

float falloff(float x, float innerAngle, float outerAngle){
    return -2.f * pow(max(0.f, (x - innerAngle)) / (outerAngle - innerAngle), 3) + 3.f * pow((x - innerAngle) / (outerAngle - innerAngle), 2);
}


void main() {
    fragColor = vec4(0.0);
    fragColor = k_a * ambientColor;
    vec3 E = normalize(vec3(worldCameraPos) - worldPosition);

    for (int i = 0; i < lightsNum; i++){
        if (lightType[i] == 0){
            vec3 L = vec3(-normalize(worldLightDir[i]));
            fragColor += (k_d * diffuseColor) * clamp(dot(L, normalize(worldNormal)), 0.0, 1.f) * worldLightCol[i];

            vec3 R = reflect(-L, normalize(worldNormal));
            fragColor += clamp((k_s * specularColor) * clamp(pow(dot(R, E), shininess), 0.f, 1.f) * worldLightCol[i], 0.f, 1.f);
        } else if (lightType[i] == 1){
             vec3 L = vec3(normalize(worldLightPos[i] - vec4(worldPosition, 1.f)));
             vec4 diffuse = (k_d * diffuseColor) * clamp(dot(L, normalize(worldNormal)), 0.0, 1.f) * worldLightCol[i];

             vec3 R = -reflect(L, normalize(worldNormal));
             vec4 specular = clamp((k_s * specularColor) * pow(clamp(dot(R, E), 0.f, 1.f), shininess) * worldLightCol[i], 0.f, 1.f);

             float distance = distance(worldLightPos[i], vec4(worldPosition, 1.f));
             float attenuation = 1.f / (lightFunction[i][0] + (distance * lightFunction[i][1]) + (lightFunction[i][2] * pow(distance, 2)));
             fragColor += attenuation * (diffuse + specular);
        } else{
            vec3 L = vec3(normalize(worldLightPos[i] - vec4(worldPosition, 1.f)));
            vec3 normalDirection = -vec3(normalize(worldLightDir[i]));
            float posToLightAngle = acos(clamp(dot(normalDirection, L), 0.f, 1.f));
            float innerAngle = lightAngle[i] - lightPenumbra[i];
            vec4 diffuse = (k_d * diffuseColor) * clamp(dot(L, normalize(worldNormal)), 0.0, 1.f) * worldLightCol[i];

            vec3 R = -reflect(L, normalize(worldNormal));
            vec4 specular = clamp((k_s * specularColor) * pow(clamp(dot(R, E), 0.f, 1.f), shininess) * worldLightCol[i], 0.f, 1.f);

            float distance = distance(worldLightPos[i], vec4(worldPosition, 1.f));
            float attenuation = 1.f / (lightFunction[i][0] + (distance * lightFunction[i][1]) + (lightFunction[i][2] * pow(distance, 2)));

             vec4 result = attenuation * (diffuse + specular);

             if(posToLightAngle <= innerAngle){
                fragColor += result;
             } else if (innerAngle < posToLightAngle && posToLightAngle <= lightAngle[i]) {
                fragColor += result * (1.f - falloff(posToLightAngle, innerAngle, lightAngle[i]));
             } else{
                fragColor += 0.f;
             }
        }
    }
}
