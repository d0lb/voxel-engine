#version 460 core
out vec4 FragColor;

uniform vec3 uColor;                         // we can change color from C++

void main() {
    FragColor = vec4(uColor, 1.0);
}