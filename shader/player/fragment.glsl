#version 410 core

out vec3 color;

in vec2 UV;

void main() {
    color = vec3(1, UV);
}