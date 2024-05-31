#ifndef MESH_MODEL_H
#define MESH_MODEL_H
#include <vector>
#include <string>
#include <glew.h>
#include <tiny_obj_loader.h>
#include "VertexStandard.h"
#include <gtc/random.hpp>
class Mesh_model {
public:
    Mesh_model(const std::string& FilePath);
    ~Mesh_model();

    void Update(float DeltaTime);
    void Render(GLuint Program, const glm::mat4& MVP, GLuint textureID);
    void InitInstances(const std::vector<glm::mat4>& instances);
    void RenderInstanced(GLuint Program, const glm::mat4& ViewProjection, GLuint textureID, int instanceCount);
    void GenerateInstanceTransforms(std::vector<glm::mat4>& transforms, int numInstances);
private:
    GLuint VAO, VBO, instanceVBO;
    GLsizei DrawCount;
    glm::vec3 statuePosition;
    float safeRadius;
};

#endif // MESH_MODEL_H
