#version 330 core
in vec4 vert;
in float worldHeight;
in vec4 norm;
in vec3 color;
in vec2 uv_coord;

uniform bool wireshade;
uniform vec3 lightDir;

uniform sampler2D rocks_texture;
uniform sampler2D sandgrass_texture;

out vec4 fragColor;

void main(void)
{
    if (wireshade) {
        fragColor = vec4(color,1);
    } else {
        vec3 objColor = color;
        fragColor = vec4((clamp(dot(norm.xyz, lightDir), 0, 1) * 0.7 +  0.3) * objColor, 1.0);
    }

    if (worldHeight < -0.1) {
        fragColor = texture(rocks_texture, uv_coord);
    } else if (worldHeight > -0.1 && worldHeight < 0.1) {
        // lerp between textures
        fragColor = mix(texture(rocks_texture, uv_coord), texture(sandgrass_texture, uv_coord), (worldHeight + 0.1) / 0.2);
    } else {
        fragColor = texture(sandgrass_texture, uv_coord);
    }

//
//    int uvx = int(uv_coord.x);
//    int uvy = int(uv_coord.y);
//    fragColor = vec4(uv_coord.x - uvx, uv_coord.y - uvy, 0, 1.f);
}
