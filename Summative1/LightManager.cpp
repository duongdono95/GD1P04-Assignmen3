#include "LightManager.h"
#include <gtc/type_ptr.hpp>
#include <glew.h>
#include <string>

LightManager::LightManager() : AmbientStrength(0.15f), AmbientColor(1.0f, 1.0f, 1.0f), PointLightCount(0) {}

void LightManager::AddPointLight(const glm::vec3& position, const glm::vec3& color, float specularStrength, float attConst, float attLinear, float attExp) {
    if (PointLightCount < MAX_POINT_LIGHTS) {
        PointLightArray[PointLightCount++] = { position, color, specularStrength, attConst, attLinear, attExp };
    }
}

void LightManager::SendToShader(GLuint program) {
    glUniform1f(glGetUniformLocation(program, "AmbientStrength"), AmbientStrength);
    glUniform3fv(glGetUniformLocation(program, "AmbientColor"), 1, glm::value_ptr(AmbientColor));
    glUniform1ui(glGetUniformLocation(program, "PointLightCount"), PointLightCount);
    for (unsigned int i = 0; i < PointLightCount; ++i) {
        SendPointLight(program, i);
    }
}

void LightManager::SendPointLight(GLuint program, int index) {
    std::string base = "PointLightArray[" + std::to_string(index) + "].";
    glUniform3fv(glGetUniformLocation(program, (base + "Position").c_str()), 1, glm::value_ptr(PointLightArray[index].Position));
    glUniform3fv(glGetUniformLocation(program, (base + "Color").c_str()), 1, glm::value_ptr(PointLightArray[index].Color));
    glUniform1f(glGetUniformLocation(program, (base + "SpecularStrength").c_str()), PointLightArray[index].SpecularStrength);
    glUniform1f(glGetUniformLocation(program, (base + "AttenuationConstant").c_str()), PointLightArray[index].AttenuationConstant);
    glUniform1f(glGetUniformLocation(program, (base + "AttenuationLinear").c_str()), PointLightArray[index].AttenuationLinear);
    glUniform1f(glGetUniformLocation(program, (base + "AttenuationExponent").c_str()), PointLightArray[index].AttenuationExponent);
}

