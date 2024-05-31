#version 460 core
layout(location=0) in vec3 Position;
uniform mat4 VP;
out vec3 FragTexCoords;

void main() {
    gl_Position = VP * vec4(Position, 1.0f);
    FragTexCoords = Position;
}
