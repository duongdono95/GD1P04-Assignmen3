#version 460 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoords;
layout(location = 2) in vec3 normal;

uniform mat4 MVP;
uniform mat4 Model;
uniform mat4 NormalMatrix;

out vec2 FragTexCoords;
out vec3 FragNormal;
out vec3 FragPosition;

void main() {
    FragTexCoords = texCoords;
    FragNormal = mat3(NormalMatrix) * normal;
    FragPosition = vec3(Model * vec4(position, 1.0));
    gl_Position = MVP * vec4(position, 1.0);
}
