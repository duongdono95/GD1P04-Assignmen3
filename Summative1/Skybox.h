// Skybox.h
#ifndef SKYBOX_H
#define SKYBOX_H

#include <glew.h>
#include <glm.hpp>
#include <vector>
#include <string>

class CCamera;

class Skybox {
public:
    Skybox(CCamera* camera, const std::vector<std::string>& filePaths);
    void Render();
    GLuint GetTextureID() const { return TextureID; }
private:
    void Initialize(const std::vector<std::string>& filePaths);

    GLuint VAO, VBO, EBO;
    GLuint TextureID;
    GLuint Program;
    CCamera* camera;
};

#endif // SKYBOX_H
