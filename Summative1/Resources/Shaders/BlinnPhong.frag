#version 460 core

in vec2 FragTexCoords;
in vec3 FragNormal;
in vec3 FragPosition;

uniform sampler2D Texture0;
uniform sampler2D ReflectionMap; // Add uniform for reflection map
uniform samplerCube SkyboxTexture;

uniform float AmbientStrength = 0.15f;
uniform vec3 AmbientColor = vec3(1.0f, 1.0f, 1.0f);

uniform vec3 CameraPos;
uniform float ObjectShininess = 32.0f;

uniform bool UseSolidColor;
uniform vec3 SolidColor;
uniform bool IsEmissive;
uniform vec3 EmissiveColor;

#define MAX_POINT_LIGHTS 4

struct PointLight {
    vec3 Position;
    vec3 Color;
    float SpecularStrength;
    float AttenuationConstant;
    float AttenuationLinear;
    float AttenuationExponent;
};

struct DirectionLight {
    vec3 Direction;
    vec3 Color;
    float SpecularStrength;
};

struct SpotLight {
    vec3 Position;
    vec3 Direction;
    vec3 Color;
    float CutOff;
    float OuterCutOff;
    float SpecularStrength;
    float AttenuationConstant;
    float AttenuationLinear;
    float AttenuationExponent;
};

uniform PointLight PointLightArray[MAX_POINT_LIGHTS];
uniform uint PointLightCount;
uniform DirectionLight DirectionalLight;
uniform SpotLight Spotlight;
uniform bool SpotlightEnabled;

out vec4 FinalColor;

vec3 CalculateLight_Point(uint index) {
    PointLight light = PointLightArray[index];
    
    vec3 ambient = AmbientStrength * AmbientColor;

    vec3 Normal = normalize(FragNormal);
    vec3 LightDir = normalize(light.Position - FragPosition);

    float DiffuseStrength = max(dot(Normal, LightDir), 0.0f);
    vec3 diffuse = DiffuseStrength * light.Color;

    vec3 ViewDir = normalize(CameraPos - FragPosition);
    vec3 HalfwayVector = normalize(LightDir + ViewDir);
    float SpecularReflectivity = pow(max(dot(Normal, HalfwayVector), 0.0f), ObjectShininess);
    vec3 specular = light.SpecularStrength * SpecularReflectivity * light.Color;

    float distance = length(light.Position - FragPosition);
    float attenuation = light.AttenuationConstant + light.AttenuationLinear * distance + light.AttenuationExponent * distance * distance;

    return (ambient + diffuse + specular) / attenuation;
}

vec3 CalculateLight_Direction() {
    DirectionLight light = DirectionalLight;
    
    vec3 ambient = AmbientStrength * AmbientColor;

    vec3 Normal = normalize(FragNormal);
    vec3 LightDir = normalize(-light.Direction); // Light direction should be normalized and inverted

    float DiffuseStrength = max(dot(Normal, LightDir), 0.0f);
    vec3 diffuse = DiffuseStrength * light.Color;

    vec3 ViewDir = normalize(CameraPos - FragPosition);
    vec3 HalfwayVector = normalize(LightDir + ViewDir);
    float SpecularReflectivity = pow(max(dot(Normal, HalfwayVector), 0.0f), ObjectShininess);
    vec3 specular = light.SpecularStrength * SpecularReflectivity * light.Color;

    return ambient + diffuse + specular;
}

vec3 CalculateLight_Spot() {
    SpotLight light = Spotlight;
    
    vec3 ambient = AmbientStrength * AmbientColor;

    vec3 Normal = normalize(FragNormal);
    vec3 LightDir = normalize(light.Position - FragPosition);
    float theta = dot(LightDir, normalize(-light.Direction));

    if(theta > light.CutOff) {
        float epsilon = light.CutOff - light.OuterCutOff;
        float intensity = clamp((theta - light.OuterCutOff) / epsilon, 0.0, 1.0);

        float DiffuseStrength = max(dot(Normal, LightDir), 0.0f);
        vec3 diffuse = DiffuseStrength * light.Color * intensity;

        vec3 ViewDir = normalize(CameraPos - FragPosition);
        vec3 HalfwayVector = normalize(LightDir + ViewDir);
        float SpecularReflectivity = pow(max(dot(Normal, HalfwayVector), 0.0f), ObjectShininess);
        vec3 specular = light.SpecularStrength * SpecularReflectivity * light.Color * intensity;

        float distance = length(light.Position - FragPosition);
        float attenuation = light.AttenuationConstant + light.AttenuationLinear * distance + light.AttenuationExponent * distance * distance;

        return (ambient + diffuse + specular) / attenuation;
    }

    return ambient;
}

void main() {
    vec3 TotalLightOutput = vec3(0.0f);
    for (uint i = 0; i < PointLightCount; i++) {
        TotalLightOutput += CalculateLight_Point(i);
    }
    TotalLightOutput += CalculateLight_Direction();

    if (SpotlightEnabled) {
        TotalLightOutput += CalculateLight_Spot();
    }

    vec3 color;
    if (UseSolidColor) {
        color = SolidColor;
    } else {
        color = texture(Texture0, FragTexCoords).rgb;
    }

    // Reflection calculation
    vec3 Normal = normalize(FragNormal);
    vec3 ViewDir = normalize(FragPosition - CameraPos);
    vec3 ReflectDir = reflect(ViewDir, Normal);
    vec3 reflection = texture(SkyboxTexture, ReflectDir).rgb;

    // Sample reflection map
    float reflectionIntensity = texture(ReflectionMap, FragTexCoords).r;

    // Mix the object texture with the reflection texture using reflection map's red channel
    vec3 mixedColor = mix(color, reflection, reflectionIntensity);

    vec3 emissive = IsEmissive ? EmissiveColor : vec3(0.0f);

    FinalColor = vec4(TotalLightOutput * mixedColor + emissive, 1.0f);
}
