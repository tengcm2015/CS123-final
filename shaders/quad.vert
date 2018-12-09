#version 400 core

layout(location = 0) in vec2 position;
layout(location = 5) in vec2 inUV;

out vec2 clipCoord;
out vec2 uv;

void main() {
    clipCoord = position;
    uv = inUV;
    gl_Position = vec4(position, 0.0, 1.0);
}
