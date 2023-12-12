#version 330 core
out vec4 FragColor;

in vec3 texCoords;

uniform samplerCube skybox;
uniform float timeofday;

void main()
{
    FragColor = texture(skybox, texCoords);
    FragColor = vec4(clamp(0.f, 1.f, FragColor[0] - (timeofday * 0.02)),
                    clamp(0.f, 1.f, FragColor[1] - (timeofday * 0.02)),
                    clamp(0.f, 1.f, FragColor[2] - (timeofday * 0.02)), FragColor[3]);
}
