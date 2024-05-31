#include "MeshModel.h"
#include <iostream>
#include "ShaderLoader.h"
#include <gtc/type_ptr.hpp>

Mesh_model::Mesh_model(const std::string& FilePath)
    : DrawCount(0) {
    std::vector<VertexStandard> Vertices;
    tinyobj::ObjReader Reader;
    tinyobj::ObjReaderConfig ReaderConfig;

    if (!Reader.ParseFromFile(FilePath, ReaderConfig)) {
        if (!Reader.Error().empty()) {
            std::cerr << "TinyObjReader: " << Reader.Error() << std::endl;
        }
        exit(1);
    }

    if (!Reader.Warning().empty()) {
        std::cout << "TinyObjReader: " << Reader.Warning() << std::endl;
    }

    auto& Attrib = Reader.GetAttrib();
    auto& Shapes = Reader.GetShapes();

    for (size_t ShapeIndex = 0; ShapeIndex < Shapes.size(); ++ShapeIndex) {
        const auto& Shape = Shapes[ShapeIndex];
        size_t ReadIndexOffset = 0;

        for (size_t FaceIndex = 0; FaceIndex < Shape.mesh.num_face_vertices.size(); ++FaceIndex) {
            size_t FaceVertexCount = size_t(Shape.mesh.num_face_vertices[FaceIndex]);

            for (size_t VertexIndex = 0; VertexIndex < FaceVertexCount; ++VertexIndex) {
                VertexStandard Vertex{};
                tinyobj::index_t TinyObjVertex = Shape.mesh.indices[ReadIndexOffset + VertexIndex];

                Vertex.position = {
                    Attrib.vertices[3 * size_t(TinyObjVertex.vertex_index) + 0],
                    Attrib.vertices[3 * size_t(TinyObjVertex.vertex_index) + 1],
                    Attrib.vertices[3 * size_t(TinyObjVertex.vertex_index) + 2]
                };

                if (TinyObjVertex.texcoord_index >= 0) {
                    Vertex.texcoord = {
                        Attrib.texcoords[2 * size_t(TinyObjVertex.texcoord_index) + 0],
                        Attrib.texcoords[2 * size_t(TinyObjVertex.texcoord_index) + 1]
                    };
                }

                if (TinyObjVertex.normal_index >= 0) {
                    Vertex.normal = {
                        Attrib.normals[3 * size_t(TinyObjVertex.normal_index) + 0],
                        Attrib.normals[3 * size_t(TinyObjVertex.normal_index) + 1],
                        Attrib.normals[3 * size_t(TinyObjVertex.normal_index) + 2]
                    };
                }

                Vertices.push_back(Vertex);
            }

            ReadIndexOffset += FaceVertexCount;
        }
    }

    DrawCount = static_cast<GLsizei>(Vertices.size());

    // OpenGL buffer setup
    GLuint VBO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(VertexStandard), &Vertices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexStandard), (void*)offsetof(VertexStandard, position));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexStandard), (void*)offsetof(VertexStandard, texcoord));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(VertexStandard), (void*)offsetof(VertexStandard, normal));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}
Mesh_model::~Mesh_model() {
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
}

void Mesh_model::Update(float DeltaTime) {

}

void Mesh_model::Render(GLuint Program, const glm::mat4& MVP, GLuint textureID) {
    glUseProgram(Program);

    GLint MVP_Location = glGetUniformLocation(Program, "MVP");
    glUniformMatrix4fv(MVP_Location, 1, GL_FALSE, glm::value_ptr(MVP));

    GLint Model_Location = glGetUniformLocation(Program, "Model");
    glm::mat4 ModelMat = glm::mat4(1.0f);  // Assuming identity matrix for now
    glUniformMatrix4fv(Model_Location, 1, GL_FALSE, glm::value_ptr(ModelMat));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glUniform1i(glGetUniformLocation(Program, "Texture0"), 0);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, DrawCount);
    glBindVertexArray(0);

    glUseProgram(0);
}

void Mesh_model::InitInstances(const std::vector<glm::mat4>& instances) {
    glBindVertexArray(VAO);
    glGenBuffers(1, &instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, instances.size() * sizeof(glm::mat4), &instances[0], GL_STATIC_DRAW);

    for (int i = 0; i < 4; i++) {
        glEnableVertexAttribArray(3 + i);
        glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4) * i));
        glVertexAttribDivisor(3 + i, 1);
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Mesh_model::RenderInstanced(GLuint Program, const glm::mat4& ViewProjection, GLuint textureID, int instanceCount) {
    glUseProgram(Program);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, statuePosition + glm::vec3(0.0f, 10.0f, 0.0f));
    model = glm::scale(model, glm::vec3(5.0f, 5.0f, 5.0f));

    GLint vpLoc = glGetUniformLocation(Program, "viewProjectionMatrix");
    glUniformMatrix4fv(vpLoc, 1, GL_FALSE, glm::value_ptr(ViewProjection));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glUniform1i(glGetUniformLocation(Program, "Texture0"), 0);

    glBindVertexArray(VAO);
    glDrawArraysInstanced(GL_TRIANGLES, 0, DrawCount, instanceCount);
    glBindVertexArray(0);

    glUseProgram(0);
}

void  Mesh_model::GenerateInstanceTransforms(std::vector<glm::mat4>& transforms, int numInstances) {
    transforms.resize(numInstances);
    glm::vec3 baseScale(0.01f);
    for (int i = 0; i < numInstances; i++) {
        glm::vec3 position;
        bool isSafe = false;
        while (!isSafe) {
            position = glm::linearRand(glm::vec3(-50, 0, -50), glm::vec3(50, 0, 50));
            float distance = glm::distance(glm::vec2(position.x, position.z), glm::vec2(statuePosition.x, statuePosition.z));
            if (distance > safeRadius) {
                isSafe = true;
            }
        }

        float angle = glm::linearRand(0.0f, 360.0f);
        float scaleMultiplier = glm::linearRand(0.5f, 1.5f);
        glm::vec3 scale = baseScale * scaleMultiplier;

        glm::mat4 model = glm::translate(glm::mat4(1.0f), position) *
            glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0)) *
            glm::scale(glm::mat4(1.0f), scale);
        transforms[i] = model;
    }
}
