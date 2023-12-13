#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv_coordinate;

out vec2 uv_coord;

void main() {
    uv_coord = uv_coordinate;
    gl_Position = vec4(position, 1.0);
}
