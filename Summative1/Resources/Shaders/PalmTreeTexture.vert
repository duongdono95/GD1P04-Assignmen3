#version 460 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoords;
layout(location = 2) in vec3 normal;
layout(location = 3) in mat4 instanceMatrix;

uniform mat4 viewProjectionMatrix; // Combined View and Projection Matrix
uniform mat4 NormalMatrix;

out vec2 FragTexCoords;
out vec3 FragNormal;
out vec3 FragPosition;

void main() {
    FragTexCoords = texCoords;
    FragNormal = mat3(NormalMatrix) * normal;
    FragPosition = vec3(instanceMatrix * vec4(position, 1.0));
    gl_Position = viewProjectionMatrix * instanceMatrix * vec4(position, 1.0);
}
