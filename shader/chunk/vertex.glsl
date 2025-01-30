#version 410 core

layout(location=0) in uint vertexIndex;
layout(location=1) in vec3 chunkOffset;
layout(location=2) in uint textureLayer;

out vec2 UV;
flat out uint textLayer;

uniform mat4 MVP;

const vec3 voxelModelPosition[36] = vec3[36](
    vec3(0, 0, 0), vec3(0, 0, 1), vec3(0, 1, 1),
	vec3(0, 0, 0), vec3(0, 1, 1), vec3(0, 1, 0),
    vec3(1, 1, 1), vec3(1, 0, 0), vec3(1, 1, 0),
	vec3(1, 0, 0), vec3(1, 1, 1), vec3(1, 0, 1),
	vec3(1, 0, 1), vec3(0, 0, 1), vec3(0, 0, 0),
	vec3(1, 0, 1), vec3(0, 0, 0), vec3(1, 0, 0),
	vec3(1, 1, 1), vec3(1, 1, 0), vec3(0, 1, 0),
	vec3(1, 1, 1), vec3(0, 1, 0), vec3(0, 1, 1),
	vec3(1, 1, 0), vec3(0, 0, 0), vec3(0, 1, 0),
	vec3(1, 1, 0), vec3(1, 0, 0), vec3(0, 0, 0),
	vec3(0, 1, 1), vec3(0, 0, 1), vec3(1, 0, 1),
    vec3(1, 1, 1), vec3(0, 1, 1), vec3(1, 0, 1)
);

const vec2 voxelModelUV[36] = vec2[36](
    vec2(0, 0), vec2(1, 0), vec2(1, 1),
    vec2(0, 0), vec2(1, 1), vec2(0, 1),
    vec2(0, 1), vec2(1, 0), vec2(1, 1),
    vec2(1, 0), vec2(0, 1), vec2(0, 0),
    vec2(0, 1), vec2(0, 0), vec2(1, 0),
    vec2(0, 1), vec2(1, 0), vec2(1, 1),
    vec2(0, 1), vec2(0, 0), vec2(1, 0),
    vec2(0, 1), vec2(1, 0), vec2(1, 1),
    vec2(0, 1), vec2(1, 0), vec2(1, 1),
    vec2(0, 1), vec2(0, 0), vec2(1, 0),
    vec2(0, 1), vec2(0, 0), vec2(1, 0),
    vec2(1, 1), vec2(0, 1), vec2(1, 0)
);

void main() {
    gl_Position = MVP * vec4(chunkOffset + voxelModelPosition[vertexIndex], 1);

    UV = voxelModelUV[vertexIndex]; 
    textLayer = textureLayer;
}