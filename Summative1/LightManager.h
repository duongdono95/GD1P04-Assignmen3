#ifndef LIGHTMANAGER_H
#define LIGHTMANAGER_H

#include <glm.hpp>
#include <vector>
#include <glew.h>
#include <glfw3.h>
struct PointLight {
    glm::vec3 Position;
    glm::vec3 Color;
    float SpecularStrength;
    float AttenuationConstant;
    float AttenuationLinear;
    float AttenuationExponent;
};
struct DirectionalLight {
    glm::vec3 Direction;
    glm::vec3 Color;
    float SpecularStrength;
};
struct SpotLight {
    glm::vec3 Position;
    glm::vec3 Direction;
    glm::vec3 Color;
    float CutOff;
    float OuterCutOff;
    float SpecularStrength;
    float AttenuationConstant;
    float AttenuationLinear;
    float AttenuationExponent;
};
class LightManager {
public:
    LightManager();
    void AddPointLight(const glm::vec3& position, const glm::vec3& color, float specularStrength, float attConst, float attLinear, float attExp);
    void SendToShader(GLuint program);
    float AmbientStrength;
    glm::vec3 AmbientColor;
    static const int MAX_POINT_LIGHTS = 4;
    PointLight PointLightArray[MAX_POINT_LIGHTS];
    unsigned int PointLightCount;

private:
    void SendPointLight(GLuint program, int index);
};

#endif // LIGHTMANAGER_H
