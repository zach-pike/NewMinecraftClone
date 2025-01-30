#version 410 core

flat in uint textLayer;
in vec2 UV;

out vec3 color;

uniform sampler2DArray samp;

void main() {
    color = texture(samp, vec3(vec2(UV.x, 1 - UV.y), textLayer)).rgb;
}