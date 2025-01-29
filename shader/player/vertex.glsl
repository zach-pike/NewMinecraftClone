#version 410 core

layout(location=0) in vec3 vertexPosition;
layout(location=1) in vec2 uv;


uniform vec3 position;
uniform mat4 MVP;

out vec2 UV;

void main() {
    UV = uv;

    gl_Position = MVP * vec4(vertexPosition + position, 1);
}