#include <glew.h>
#include <glfw3.h>
#include <vector>
#include <string>
#include "ShaderLoader.h"
#include "LightManager.h"
#include "CCamera.h"
#include "Input.h"
#include "Skybox.h"
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "VertexStandard.h"
#include "MeshModel.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include <gtc/matrix_inverse.hpp>

// ---------------------------- Global Variable ------------------------------------------- //
CCamera camera(glm::vec3(0.0f, 0.0f, 100.0f), 800.0f / 600.0f);
GLFWwindow* window = nullptr;

// Programs
GLuint Program_Palm_Tree_Texture;
GLuint Texture_Acient_World;
GLuint Texture_Model;
int World_ImageWidth, World_ImageHeight, World_ImageComponents;

// Models
Mesh_model* statueModel;
glm::vec3 statuePosition = glm::vec3(0.0f, 0.0f, 0.0f);
float safeRadius = 10.0f;


Mesh_model* sphereModelLeft;
Mesh_model* sphereModelRight;

Mesh_model* palmTreeModel;
std::vector<GLuint> textures;
std::vector<glm::mat4> instanceTransforms;
int NumberPalmTree = 500;

Mesh_model* shipModel;
glm::vec3 shipPosition = glm::vec3(0.0f, 20.0f, 0.0f);
GLuint Texture_Ship;
int Ship_ImageWidth, Ship_ImageHeight, Ship_ImageComponents;

GLuint Texture_ReflectionMap;

// Lights
bool pointLightsEnabled = true;
LightManager lightManager;

bool directionalLightEnabled = false;
DirectionalLight directionalLight = {
    glm::vec3(-1.0f, -1.0f, 0.0f),
    glm::vec3(1.0f, 1.0f, 1.0f),
    0.5f
};

bool spotlightEnabled = false;
SpotLight spotlight = {
    glm::vec3(0.0f, 0.0f, 0.0f),
    glm::vec3(0.0f, 0.0f, -1.0f),
    glm::vec3(1.0f, 1.0f, 1.0f),
    glm::cos(glm::radians(12.5f)),
    glm::cos(glm::radians(17.5f)),
    0.5f,
    1.0f,
    0.022f,
    0.0019f
};

Skybox* skybox;
extern Skybox* skybox;
// Function declarations
void SceneSetup();
void InitialSetup();
void Update(float deltaTime);
void Render();


int main() {
    SceneSetup();
    Input::Initialize(window);
    InitialSetup();
    float lastFrame = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        // ----------------------------------- Camera Adjustment Based on Input --------------------------- //
        float currentFrameTime = glfwGetTime();
        float deltaTime = currentFrameTime - lastFrame;
        lastFrame = currentFrameTime;

        Update(deltaTime);

        Render();
        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR) {
            std::cerr << "OpenGL error: " << err << std::endl;
        }
        glfwSwapBuffers(window);
    }

    // Clean up
    delete shipModel;
    delete skybox;
    delete statueModel;
    delete palmTreeModel;
    delete sphereModelLeft;
    delete sphereModelRight;
    glDeleteTextures(1, &Texture_Ship);
    for (GLuint tex : textures) {
        glDeleteTextures(1, &tex);
    }
    glfwTerminate();
    return 0;
}

void InitialSetup() {
    // ---------------------------------------- SHADER PROGRAM ---------------------------------------- //
    Texture_Acient_World = ShaderLoader::CreateProgram("Resources/Shaders/BlinnPhong.vert", "Resources/Shaders/BlinnPhong.frag");
    Program_Palm_Tree_Texture = ShaderLoader::CreateProgram("Resources/Shaders/PalmTreeTexture.vert", "Resources/Shaders/BlinnPhong.frag");

    // ---------------------------------------- IMPORT MODELS ---------------------------------------- //
    statueModel = new Mesh_model("Resources/Models/AncientEmpire/SM_Prop_Statue_01.obj");
    palmTreeModel = new Mesh_model("Resources/Models/AncientEmpire/SM_Env_Tree_Palm_01.obj");
    sphereModelLeft = new Mesh_model("Resources/Models/Sphere.obj");
    sphereModelRight = new Mesh_model("Resources/Models/Sphere.obj");
    shipModel = new Mesh_model("Resources/Models/SpaceShip/SM_Ship_Fighter_02.obj");

    // ----------------------------------------- SETUP Image Textures ---------------------------------------------------------------- //
    // ----------------------------------------- Ancient World Texture
    stbi_set_flip_vertically_on_load(true);
    unsigned char* ImageData0 = stbi_load("Resources/Textures/PolygonAncientWorlds_Texture_01_B.png", &World_ImageWidth, &World_ImageHeight, &World_ImageComponents, 0);
    if (ImageData0 == nullptr) {
        std::cerr << "Failed to load texture!" << std::endl;
    }
    else {
        std::cout << "Texture loaded: " << World_ImageWidth << "x" << World_ImageHeight << " components: " << World_ImageComponents << std::endl;
    }
    glGenTextures(1, &Texture_Model);
    glBindTexture(GL_TEXTURE_2D, Texture_Model);
    GLint LoadedComponent0 = (World_ImageComponents == 4) ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, LoadedComponent0, World_ImageWidth, World_ImageHeight, 0, LoadedComponent0, GL_UNSIGNED_BYTE, ImageData0);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(ImageData0);

    glBindTexture(GL_TEXTURE_2D, 0);
    // ------------------------------------------ Prepare for PalmTree Rendering ------------------------------------------ // 
    palmTreeModel->GenerateInstanceTransforms(instanceTransforms, NumberPalmTree);
    palmTreeModel->InitInstances(instanceTransforms);
    // Setup lights
    lightManager.AddPointLight(glm::vec3(25.0f, 15.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), 1.0f, 1.0f, 0.045f, 0.0075f);
    lightManager.AddPointLight(glm::vec3(-25.0f, 15.0f, 0.0f), glm::vec3(1.0f, 0.0f, 1.0f), 1.0f, 1.0f, 0.045f, 0.0075f);

    // Skybox
    std::vector<std::string> skyboxFilePaths = {
         "Resources/Textures/Corona/Left.png",
         "Resources/Textures/Corona/Front.png",
         "Resources/Textures/Corona/Right.png",
         "Resources/Textures/Corona/Back.png",
         "Resources/Textures/Corona/Top.png",
         "Resources/Textures/Corona/Bottom.png",
    };
    skybox = new Skybox(&camera, skyboxFilePaths);
    // Load the ship texture
    unsigned char* ImageData1 = stbi_load("Resources/Models/SpaceShip/PolygonSciFiSpace_Texture_01_A.png", &Ship_ImageWidth, &Ship_ImageHeight, &Ship_ImageComponents, 0);
    if (ImageData1 == nullptr) {
        std::cerr << "Failed to load ship texture!" << std::endl;
    }
    else {
        std::cout << "Ship texture loaded: " << Ship_ImageWidth << "x" << Ship_ImageHeight << " components: " << Ship_ImageComponents << std::endl;
    }
    glGenTextures(1, &Texture_Ship);
    glBindTexture(GL_TEXTURE_2D, Texture_Ship);
    GLint LoadedComponent1 = (Ship_ImageComponents == 4) ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, LoadedComponent1, Ship_ImageWidth, Ship_ImageHeight, 0, LoadedComponent1, GL_UNSIGNED_BYTE, ImageData1);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(ImageData1);

    glBindTexture(GL_TEXTURE_2D, 0);

    // Load the reflection map texture
    unsigned char* ImageData2 = stbi_load("Resources/Models/SpaceShip/ReflectionMap_White.png", &Ship_ImageWidth, &Ship_ImageHeight, &Ship_ImageComponents, 0);
    if (ImageData2 == nullptr) {
        std::cerr << "Failed to load reflection map texture!" << std::endl;
    }
    else {
        std::cout << "Reflection map texture loaded: " << Ship_ImageWidth << "x" << Ship_ImageHeight << " components: " << Ship_ImageComponents << std::endl;
    }
    glGenTextures(1, &Texture_ReflectionMap);
    glBindTexture(GL_TEXTURE_2D, Texture_ReflectionMap);
    GLint LoadedComponent2 = (Ship_ImageComponents == 4) ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, LoadedComponent2, Ship_ImageWidth, Ship_ImageHeight, 0, LoadedComponent2, GL_UNSIGNED_BYTE, ImageData2);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(ImageData2);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void Update(float deltaTime) {
    glfwPollEvents();
    Input::Update();

    // Toggle point lights
    if (Input::IsKeyToggled(GLFW_KEY_1)) {
        pointLightsEnabled = !pointLightsEnabled;
    }

    // Toggle directional light
    if (Input::IsKeyToggled(GLFW_KEY_2)) {
        directionalLightEnabled = !directionalLightEnabled;
    }

    // Toggle spotlight
    if (Input::IsKeyToggled(GLFW_KEY_3)) {
        spotlightEnabled = !spotlightEnabled;
    }

    // Update spotlight position and direction based on camera
    spotlight.Position = camera.GetPosition();
    spotlight.Direction = camera.GetForwardVector();

    // Camera toggle Manual/Auto Mode
    if (Input::IsKeyPressed(GLFW_KEY_SPACE)) {
        camera.SetMode(camera.GetMode() == CCamera::MANUAL ? CCamera::AUTO : CCamera::MANUAL);
    }
    if (camera.GetMode() == CCamera::MANUAL) {
        float angleDelta = Input::IsKeyPressed(GLFW_KEY_RIGHT) - Input::IsKeyPressed(GLFW_KEY_LEFT);
        float radiusDelta = Input::IsKeyPressed(GLFW_KEY_UP) - Input::IsKeyPressed(GLFW_KEY_DOWN);
        if (Input::IsShiftPressed()) {
            angleDelta *= 10;
            radiusDelta *= 10;
        }
        camera.AdjustOrbit(angleDelta * 0.05f, radiusDelta);
    }
    else if (camera.GetMode() == CCamera::AUTO) {
        float speedFactor = Input::IsShiftPressed() ? 10.0f : 1.0f;
        camera.AdjustOrbit(0.02f * deltaTime * speedFactor, 0);
    }
    camera.Update(deltaTime);
}

void Render() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    skybox->Render();

    glm::mat4 ProjectionMat = camera.GetProjMat();
    glm::mat4 ViewMat = camera.GetViewMat();

    // Render statue model with texture
    glm::mat4 StatueModelMat = glm::translate(glm::mat4(1.0f), statuePosition) * glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));
    glm::mat4 StatueMVP = ProjectionMat * ViewMat * StatueModelMat;
    glm::mat4 NormalMatrix = glm::transpose(glm::inverse(StatueModelMat));

    glUseProgram(Texture_Acient_World);
    glUniformMatrix4fv(glGetUniformLocation(Texture_Acient_World, "MVP"), 1, GL_FALSE, glm::value_ptr(StatueMVP));
    glUniformMatrix4fv(glGetUniformLocation(Texture_Acient_World, "Model"), 1, GL_FALSE, glm::value_ptr(StatueModelMat));
    glUniformMatrix4fv(glGetUniformLocation(Texture_Acient_World, "NormalMatrix"), 1, GL_FALSE, glm::value_ptr(NormalMatrix));
    glUniform3fv(glGetUniformLocation(Texture_Acient_World, "CameraPos"), 1, glm::value_ptr(camera.GetPosition()));
    glUniform1i(glGetUniformLocation(Texture_Acient_World, "UseSolidColor"), GL_FALSE); // Ensure the texture is applied
    glUniform1i(glGetUniformLocation(Texture_Acient_World, "IsEmissive"), GL_FALSE);
    // condition for Directional Light
    if (directionalLightEnabled) {
        // Set directional light uniforms
        glUniform3fv(glGetUniformLocation(Texture_Acient_World, "DirectionalLight.Direction"), 1, glm::value_ptr(directionalLight.Direction));
        glUniform3fv(glGetUniformLocation(Texture_Acient_World, "DirectionalLight.Color"), 1, glm::value_ptr(directionalLight.Color));
        glUniform1f(glGetUniformLocation(Texture_Acient_World, "DirectionalLight.SpecularStrength"), directionalLight.SpecularStrength);
    }
    else {
        glUniform3fv(glGetUniformLocation(Texture_Acient_World, "DirectionalLight.Color"), 1, glm::value_ptr(glm::vec3(0.0f)));
    }
    // condition for Point Lights
    if (pointLightsEnabled) {
        lightManager.SendToShader(Texture_Acient_World);
    }
    else {
        for (int i = 0; i < lightManager.PointLightCount; ++i) {
            std::string base = "PointLightArray[" + std::to_string(i) + "].";
            glUniform3fv(glGetUniformLocation(Texture_Acient_World, (base + "Color").c_str()), 1, glm::value_ptr(glm::vec3(0.0f)));
        }
    }
    // condition for Spot Light
    if (spotlightEnabled) {
        glUniform3fv(glGetUniformLocation(Texture_Acient_World, "Spotlight.Position"), 1, glm::value_ptr(spotlight.Position));
        glUniform3fv(glGetUniformLocation(Texture_Acient_World, "Spotlight.Direction"), 1, glm::value_ptr(spotlight.Direction));
        glUniform3fv(glGetUniformLocation(Texture_Acient_World, "Spotlight.Color"), 1, glm::value_ptr(spotlight.Color));
        glUniform1f(glGetUniformLocation(Texture_Acient_World, "Spotlight.CutOff"), spotlight.CutOff);
        glUniform1f(glGetUniformLocation(Texture_Acient_World, "Spotlight.OuterCutOff"), spotlight.OuterCutOff);
        glUniform1f(glGetUniformLocation(Texture_Acient_World, "Spotlight.SpecularStrength"), spotlight.SpecularStrength);
        glUniform1f(glGetUniformLocation(Texture_Acient_World, "Spotlight.AttenuationConstant"), spotlight.AttenuationConstant);
        glUniform1f(glGetUniformLocation(Texture_Acient_World, "Spotlight.AttenuationLinear"), spotlight.AttenuationLinear);
        glUniform1f(glGetUniformLocation(Texture_Acient_World, "Spotlight.AttenuationExponent"), spotlight.AttenuationExponent);
        glUniform1i(glGetUniformLocation(Texture_Acient_World, "SpotlightEnabled"), GL_TRUE);
    }
    else {
        glUniform1i(glGetUniformLocation(Texture_Acient_World, "SpotlightEnabled"), GL_FALSE);
    }

    statueModel->Render(Texture_Acient_World, StatueMVP, Texture_Model);

    // Render Palm Tree Model with Blinn-Phong Lighting
    glm::mat4 PalmTreeViewProjection = camera.GetProjMat() * camera.GetViewMat();
    glm::mat4 PalmTreeNormalMatrix = glm::transpose(glm::inverse(glm::mat4(1.0f))); // Identity matrix for normal matrix since instances already have their transforms

    glUseProgram(Program_Palm_Tree_Texture);
    glUniformMatrix4fv(glGetUniformLocation(Program_Palm_Tree_Texture, "viewProjectionMatrix"), 1, GL_FALSE, glm::value_ptr(PalmTreeViewProjection));
    glUniformMatrix4fv(glGetUniformLocation(Program_Palm_Tree_Texture, "NormalMatrix"), 1, GL_FALSE, glm::value_ptr(PalmTreeNormalMatrix));
    glUniform3fv(glGetUniformLocation(Program_Palm_Tree_Texture, "CameraPos"), 1, glm::value_ptr(camera.GetPosition()));
    if (pointLightsEnabled) {
        lightManager.SendToShader(Program_Palm_Tree_Texture);
    }
    else {
        for (int i = 0; i < lightManager.PointLightCount; ++i) {
            std::string base = "PointLightArray[" + std::to_string(i) + "].";
            glUniform3fv(glGetUniformLocation(Program_Palm_Tree_Texture, (base + "Color").c_str()), 1, glm::value_ptr(glm::vec3(0.0f)));
        }
    }
    palmTreeModel->RenderInstanced(Program_Palm_Tree_Texture, PalmTreeViewProjection, Texture_Model, NumberPalmTree);

    // Render left sphere with solid blue color at point light 0
    glm::mat4 SphereModelMatLeft = glm::translate(glm::mat4(1.0f), lightManager.PointLightArray[0].Position) * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));
    glm::mat4 SphereMVPLeft = ProjectionMat * ViewMat * SphereModelMatLeft;
    glm::mat4 SphereNormalMatrixLeft = glm::transpose(glm::inverse(SphereModelMatLeft));

    glUseProgram(Texture_Acient_World);
    glUniformMatrix4fv(glGetUniformLocation(Texture_Acient_World, "MVP"), 1, GL_FALSE, glm::value_ptr(SphereMVPLeft));
    glUniformMatrix4fv(glGetUniformLocation(Texture_Acient_World, "Model"), 1, GL_FALSE, glm::value_ptr(SphereModelMatLeft));
    glUniformMatrix4fv(glGetUniformLocation(Texture_Acient_World, "NormalMatrix"), 1, GL_FALSE, glm::value_ptr(SphereNormalMatrixLeft));
    glUniform3fv(glGetUniformLocation(Texture_Acient_World, "CameraPos"), 1, glm::value_ptr(camera.GetPosition()));
    glUniform1i(glGetUniformLocation(Texture_Acient_World, "UseSolidColor"), GL_TRUE);
    glUniform3fv(glGetUniformLocation(Texture_Acient_World, "SolidColor"), 1, glm::value_ptr(glm::vec3(0.0f, 0.0f, 1.0f))); // Blue color
    glUniform1i(glGetUniformLocation(Texture_Acient_World, "IsEmissive"), GL_TRUE);
    glUniform3fv(glGetUniformLocation(Texture_Acient_World, "EmissiveColor"), 1, glm::value_ptr(glm::vec3(0.0f, 0.0f, 1.0f))); // Blue glow
    sphereModelLeft->Render(Texture_Acient_World, SphereMVPLeft, 0); // No texture needed

    // Render right sphere with solid purple color at point light 1
    glm::mat4 SphereModelMatRight = glm::translate(glm::mat4(1.0f), lightManager.PointLightArray[1].Position) * glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));
    glm::mat4 SphereMVPRight = ProjectionMat * ViewMat * SphereModelMatRight;
    glm::mat4 SphereNormalMatrixRight = glm::transpose(glm::inverse(SphereModelMatRight));

    glUseProgram(Texture_Acient_World);
    glUniformMatrix4fv(glGetUniformLocation(Texture_Acient_World, "MVP"), 1, GL_FALSE, glm::value_ptr(SphereMVPRight));
    glUniformMatrix4fv(glGetUniformLocation(Texture_Acient_World, "Model"), 1, GL_FALSE, glm::value_ptr(SphereModelMatRight));
    glUniformMatrix4fv(glGetUniformLocation(Texture_Acient_World, "NormalMatrix"), 1, GL_FALSE, glm::value_ptr(SphereNormalMatrixRight));
    glUniform3fv(glGetUniformLocation(Texture_Acient_World, "CameraPos"), 1, glm::value_ptr(camera.GetPosition()));
    glUniform1i(glGetUniformLocation(Texture_Acient_World, "UseSolidColor"), GL_TRUE);
    glUniform3fv(glGetUniformLocation(Texture_Acient_World, "SolidColor"), 1, glm::value_ptr(glm::vec3(1.0f, 0.0f, 1.0f))); // Purple color
    glUniform1i(glGetUniformLocation(Texture_Acient_World, "IsEmissive"), GL_TRUE);
    glUniform3fv(glGetUniformLocation(Texture_Acient_World, "EmissiveColor"), 1, glm::value_ptr(glm::vec3(1.0f, 0.0f, 1.0f))); // Purple glow
    sphereModelRight->Render(Texture_Acient_World, SphereMVPRight, 0); // No texture needed

    // Render ship model with texture and reflection
    glm::mat4 ShipModelMat = glm::translate(glm::mat4(1.0f), shipPosition) * glm::scale(glm::mat4(0.05f), glm::vec3(0.05f));
    glm::mat4 ShipMVP = ProjectionMat * ViewMat * ShipModelMat;
    glm::mat4 ShipNormalMatrix = glm::transpose(glm::inverse(ShipModelMat));

    glUseProgram(Texture_Acient_World);
    glUniformMatrix4fv(glGetUniformLocation(Texture_Acient_World, "MVP"), 1, GL_FALSE, glm::value_ptr(ShipMVP));
    glUniformMatrix4fv(glGetUniformLocation(Texture_Acient_World, "Model"), 1, GL_FALSE, glm::value_ptr(ShipModelMat));
    glUniformMatrix4fv(glGetUniformLocation(Texture_Acient_World, "NormalMatrix"), 1, GL_FALSE, glm::value_ptr(ShipNormalMatrix));
    glUniform3fv(glGetUniformLocation(Texture_Acient_World, "CameraPos"), 1, glm::value_ptr(camera.GetPosition()));
    glUniform1i(glGetUniformLocation(Texture_Acient_World, "UseSolidColor"), GL_FALSE); // Ensure the texture is applied
    glUniform1i(glGetUniformLocation(Texture_Acient_World, "IsEmissive"), GL_FALSE);

    // Pass the skybox texture to the shader
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->GetTextureID());
    glUniform1i(glGetUniformLocation(Texture_Acient_World, "SkyboxTexture"), 1);

    // Pass the reflection map texture to the shader
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, Texture_ReflectionMap);
    glUniform1i(glGetUniformLocation(Texture_Acient_World, "ReflectionMap"), 2);

    // Set the mix intensity
    glUniform1f(glGetUniformLocation(Texture_Acient_World, "MixIntensity"), 0.5f);

    shipModel->Render(Texture_Acient_World, ShipMVP, Texture_Ship);

    // Check for OpenGL errors
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "OpenGL error: " << err << std::endl;
    }
    glUseProgram(0);
}
void SceneSetup() {
    // ------------------------------------------ Setup Rendering Window ------------------------------------------------------ //
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW." << std::endl;
        exit(-1);
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(800, 800, "First OpenGL Window", NULL, NULL);
    if (window == nullptr) {
        std::cerr << "Failed to create GLFW window. Terminating Program." << std::endl;
        glfwTerminate();
        exit(-1);
    }

    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW. Terminating Program." << std::endl;
        glfwTerminate();
        exit(-1);
    }

    // Setup bg Color.
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glViewport(0, 0, 800, 800);

    // ------------------------------------------ Setup Camera ------------------------------------------------------ //
    float cameraHeight = 20.0f;  // Height of the camera above the scene
    float angleDegrees = 15.0f;  // Downward angle in degrees

    float angleRadians = glm::radians(angleDegrees);
    float offset = cameraHeight / tan(angleRadians);

    // ------------------------------------------ Depth Buffer ------------------------------------------------------ //
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
}