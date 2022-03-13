#version 330 core
layout (location = 0) in vec3 aPos;


out vec3 outColor;
uniform float depth;
uniform vec3 color;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * vec4(aPos.xy, depth, 1.0f);
    outColor = color;
}
