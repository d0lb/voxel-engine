#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUV;
layout (location = 2) in float aFaceIndex;
layout (location = 3) in float aBlockType;

out vec2 vUV;
flat out float vFaceIndex;
flat out float vBlockType;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

void main() {
    gl_Position = uProjection * uView * uModel * vec4(aPos, 1.0);
    vUV = aUV;
    vFaceIndex = aFaceIndex;
    vBlockType = aBlockType;
}