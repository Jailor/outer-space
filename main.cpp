#define STB_IMAGE_IMPLEMENTATION

// comment and uncomment in order to use NVIDIA GPU

#ifdef __cplusplus
extern "C" {
#endif

    __declspec(dllexport) int NvOptimusEnablement = 1;
    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;

#ifdef __cplusplus
}
#endif

// standard incluedes
#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>
#include <valarray>
// GLEW, GLFW and GLM includes
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


// constants and others
#include "CommonValues.h"
#include "UtilityFunctions.h"

// https://www.textures.com/ for textures
// Free3D for some 3D models to use
#include "Mesh.h"
#include "Shader.h"
#include "Window.h"
#include "Camera.h"
#include "Texture.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
#include "Model.h"
#include "Skybox.h"

#include <windows.h>
#include <mmsystem.h>


Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;
Shader omniShadowShader;
Shader directionalShadowShader;

Camera camera;

//usually have these inside the mesh
Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture rockTexture;
Texture metalTexture;
Texture glassBlue;

DirectionalLight mainLight;
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

Material shinyMaterial;
Material dullMaterial;
Material mediumMaterial;
Material groundMaterial;

Model xwing;
Model blackhawk_propeller_big;
Model blackhawk_propeller_small;
Model blackhawk_body;
Model tempest;
Model planet;
Model rock;
Model rock2;
Model mars_one;
Model moon_planet;
Model space_rock;
Model saturn;
Model fence;
Model up_base;
Model rover;
Model cubes[MAX_POINT_LIGHTS + MAX_SPOT_LIGHTS];
Model glass_dome;

Skybox skybox;
Skybox skyboxNight;
Skybox skyboxSpace;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

GLfloat blackhawkAngle = 0.0f;
GLfloat blackhawkAngleSpeed = 0.075f;

glm::mat4* modelMatricesInstanced;
glm::mat4* modelMatricesRegular;

SkyboxNames ACTIVE_SKYBOX = SkyboxNames::SPACE_SCENE;
TeleportLocations TELEPORT_LOCATION = TeleportLocations::MAIN_SCENE;

float secondsFrame;
int cnt = 0;

// uniform variables
GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
uniformSpecularIntensity = 0, uniformShininess = 0,
uniformOmniLightPos = 0, uniformFarPlane = 0;

unsigned int pointLightCount = 0;
unsigned int spotLightCount = 0;
unsigned int renderingMode = 0;

bool fogEnable = false;
bool isTeleporting = false;
GLfloat teleport_start = 0.0f;
float teleportStrength = 0.0f;

bool ENABLE_OMNIDIRECATIONAL_SHADOW_MAP = true;

bool isAnimation = false;
float animationLength = 15;
float animationStart;

// Modifiable parameters
float farPlane = 300.0f;

glm::vec3 moonOffset = glm::vec3(100.0f, 70.0f, -70.0f);

glm::vec3 roverPosition = glm::vec3(-30.0f, -1.1f, 0.0f);
glm::vec3 roverFront = glm::vec3(0.0f, 0.0f, -1.0f);
float roverAngle = 0.0f;

glm::vec3 spotlightDirection = glm::vec3(0.0f, -0.075f, 0.0f);
float spotlightAngle = 0.0f;

unsigned int amount = 200; 
unsigned int shadowMapSizeDirectional = 8192; // was 8192 | 256; max 15k
unsigned int shadowMapSizeOthers = 1024; // was 1024 | 128


// F - Toggle Spotlight on/off
// Q - Toggle full/wireframe/point
// R - Disable/enable shadows for point lights
// T - disable/enable most of the point lights in the scene
// WASD - move camera
// UP/DOWN/LEFT/RIGHT arrow keys - move the rover
// 1234 teleport
// E- activate fog and switch to night scene
void processInput() {
    // get current time for delta time calculations
    GLfloat now = glfwGetTime();
    deltaTime = now - lastTime;
    lastTime = now;

    if (isTeleporting) {
        teleportStrength = (now - teleport_start) * 0.66666f;
        if (now - teleport_start > 1.5f) {
            switch (TELEPORT_LOCATION)
            {
            case TeleportLocations::MAIN_SCENE:
                camera.setCameraPosition(glm::vec3(0, 0, 0));
                break;
            case TeleportLocations::MOON_SCENE:
                camera.setCameraPosition(glm::vec3(65.932335, 86.924324, -49.359177));
                break;
            case TeleportLocations::STAR_TREK_SCENE:
                camera.setCameraPosition(glm::vec3(-1.034248, 70.969421, -116.237755));
                break;
            case TeleportLocations::SATURN_SCENE:
                camera.setCameraPosition(glm::vec3(-218.581467, 70.822655, -6.387633));
                break;
            default:
                break;
            }
            PlaySound(TEXT("Sounds/teleport_sound.wav"), NULL, SND_FILENAME | SND_ASYNC);
            isTeleporting = false;
            teleportStrength = 0.0f;
        }
    }

    // spotlight operation
    if (mainWindow.getKeys()[GLFW_KEY_F])
    {
        spotLights[0].toggle();
        mainWindow.getKeys()[GLFW_KEY_F] = false;
    }
    // change  rendering mode: fill, wireframe, point
    if (mainWindow.getKeys()[GLFW_KEY_Q])
    {
        renderingMode++;
        if (renderingMode == 3) renderingMode = 0;
        mainWindow.getKeys()[GLFW_KEY_Q] = false;
    }
    // change night scene
    if (mainWindow.getKeys()[GLFW_KEY_E])
    {
        fogEnable = !fogEnable;
        if (fogEnable == true)
        {
            ACTIVE_SKYBOX = SkyboxNames::NIGHT_SCENE;
            skybox = skyboxNight;
            mainLight.setAmbientDiffuse(glm::vec2(0.15f, 0.0f));
            mainLight.setDirection(glm::vec3(0.0f, -150.0f,0.0f));
            groundMaterial = dullMaterial;
        }
        else
        {
            skybox = skyboxSpace;
            ACTIVE_SKYBOX = SkyboxNames::SPACE_SCENE;
            mainLight.setAmbientDiffuse(glm::vec2(0.15f, 0.35f));
            mainLight.setDirection(glm::vec3(0.0f, -150.0f, 130.0f));
            groundMaterial = mediumMaterial;
        }
        //setupSkybox(ACTIVE_SKYBOX, skybox);
      
        mainWindow.getKeys()[GLFW_KEY_E] = false;
    }
    // Teleport locations

    if (mainWindow.getKeys()[GLFW_KEY_1])
    {
        isTeleporting = true;
        TELEPORT_LOCATION = TeleportLocations::MAIN_SCENE;
        teleport_start = glfwGetTime();
        PlaySound(TEXT("Sounds/charge.wav"), NULL, SND_FILENAME | SND_ASYNC); // teleport_sound.wav | enderman.wav
        mainWindow.getKeys()[GLFW_KEY_1] = false;
    }
    if (mainWindow.getKeys()[GLFW_KEY_2])
    {
        isTeleporting = true;
        TELEPORT_LOCATION = TeleportLocations::MOON_SCENE;
        teleport_start = glfwGetTime();
        PlaySound(TEXT("Sounds/charge.wav"), NULL, SND_FILENAME | SND_ASYNC);
        mainWindow.getKeys()[GLFW_KEY_2] = false;
    }
    if (mainWindow.getKeys()[GLFW_KEY_3])
    {
        isTeleporting = true;
        TELEPORT_LOCATION = TeleportLocations::STAR_TREK_SCENE;
        teleport_start = glfwGetTime();
        PlaySound(TEXT("Sounds/charge.wav"), NULL, SND_FILENAME | SND_ASYNC);
        mainWindow.getKeys()[GLFW_KEY_3] = false;
    }
    if (mainWindow.getKeys()[GLFW_KEY_4])
    {
        isTeleporting = true;
        TELEPORT_LOCATION = TeleportLocations::SATURN_SCENE;
        teleport_start = glfwGetTime();
        PlaySound(TEXT("Sounds/charge.wav"), NULL, SND_FILENAME | SND_ASYNC);
        mainWindow.getKeys()[GLFW_KEY_4] = false;
    }

    if (mainWindow.getKeys()[GLFW_KEY_5])
    {
        isAnimation = !isAnimation;
        if(isAnimation) animationStart = glfwGetTime();
        mainWindow.getKeys()[GLFW_KEY_5] = false;
    }

    if (mainWindow.getKeys()[GLFW_KEY_T])
    {
        if (pointLightCount == 1) {
            pointLightCount = 5;
            spotLightCount = 2;
        }
        else
        {
            pointLightCount = 1;
            spotLightCount = 2;
        }
        mainWindow.getKeys()[GLFW_KEY_T] = false;
    }

    if (mainWindow.getKeys()[GLFW_KEY_R])
    {
        ENABLE_OMNIDIRECATIONAL_SHADOW_MAP = !ENABLE_OMNIDIRECATIONAL_SHADOW_MAP;
        mainWindow.getKeys()[GLFW_KEY_R] = false;
    }

    if (mainWindow.getKeys()[GLFW_KEY_P])
    {
        printf("Cam pos: glm::vec3(%f,%f,%f);\n", camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);
        printf("yaw = %f; pitch = %f\n", camera.getYaw(), camera.getPitch());
        mainWindow.getKeys()[GLFW_KEY_P] = false;
    }

}

void printFramerate()
{
    cnt++;
    secondsFrame += deltaTime;
    if (secondsFrame >= 1.0f)
    {
        printf("Fps for 1 second: %d\n", cnt);
        cnt = 0;
        secondsFrame = 0.0f;
    }
}

glm::mat4 blackhawkTransform() {
    glm::mat4 model = glm::mat4(1.0f);
    float tilt = -15.0f;
    float scale_val = 0.7f;
    float translate_x = -7.0f;
    float translate_y = 3.0f;
    float translate_z = 0.0f;

    model = glm::rotate(model, -blackhawkAngle * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, tilt * toRadians, glm::vec3(0.0f, 0.0f, 1.0f)); //tilt 
    model = glm::translate(model, glm::vec3(translate_x, translate_y, translate_z));
    model = glm::scale(model, glm::vec3(scale_val, scale_val, scale_val));
    return model;
}

void renderBlackhawk() {
    // blackhawk
    glm::mat4 model = glm::mat4(1.0f);

    model = blackhawkTransform();
    model = glm::translate(model, glm::vec3(0.256912f, 1.63451f, 7.43832f));
    model = glm::rotate(model, -blackhawkAngle * toRadians * 3, glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::translate(model, glm::vec3(-0.256912f, -1.63451f, -7.43832f));
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
    shinyMaterial.useMaterial(uniformSpecularIntensity, uniformShininess);
    blackhawk_propeller_small.renderModel();

    // blackhawk propeller big
    model = blackhawkTransform();
    model = glm::rotate(model, -blackhawkAngle * toRadians * 3, glm::vec3(0.0f, 1.0f, 0.0f));
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
    shinyMaterial.useMaterial(uniformSpecularIntensity, uniformShininess);
    blackhawk_propeller_big.renderModel();

    // blackhawk body
    model = blackhawkTransform();
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
    shinyMaterial.useMaterial(uniformSpecularIntensity, uniformShininess);
    blackhawk_body.renderModel();
    
}

void rover_move(bool* keys, GLfloat deltaTime, GLfloat moveSpeed)
{
    GLfloat velocity = moveSpeed * deltaTime;

    if (keys[GLFW_KEY_UP]) roverPosition += roverFront * velocity;
    if (keys[GLFW_KEY_DOWN]) roverPosition -= roverFront * velocity;

    // LEFT : (-36.479470, -1.100000, -0.823228) x
    // RIGHT : (-21.581999, -1.100000, 0.514411) x
    // FAR : (-30.000000, -1.100000, -19.074228) z
    // NEAR: (-27.056206, -1.100000, 10.491595)  z

    float max_x = -21.581999f;
    float min_x = -36.479470f;
    float max_z = 10.491595f;
    float min_z = -19.074228f;

    if (roverPosition.x > max_x) roverPosition.x = max_x;
    if (roverPosition.x < min_x) roverPosition.x = min_x;
    if (roverPosition.z > max_z) roverPosition.z = max_z;
    if (roverPosition.z < min_z) roverPosition.z = min_z;
}

void rover_rotate(bool* keys, GLfloat deltaTime, GLfloat moveSpeed)
{
    GLfloat velocity = moveSpeed * deltaTime;
    if (keys[GLFW_KEY_LEFT]) {
        roverAngle -= velocity;
    }
    if (keys[GLFW_KEY_RIGHT]) {
        roverAngle += velocity;
    }

    if (roverAngle > 360.0f) roverAngle -= 360.0f;
    if (roverAngle < 0.0f) roverAngle += 360.0f;

    roverFront.x = glm::sin(glm::radians(roverAngle));
    roverFront.z = -glm::cos(glm::radians(roverAngle));
}

void spotlight_rotate( GLfloat deltaTime, GLfloat moveSpeed)
{
    GLfloat velocity = moveSpeed * deltaTime;
    spotlightAngle += velocity;
    spotlightDirection.x = glm::sin(glm::radians(spotlightAngle));
    spotlightDirection.z = -glm::cos(glm::radians(spotlightAngle));

    spotLights[1].setDirection(spotlightDirection);
}

//GENERATE ANIMATION INDEPENDENT OF GRAPHICS CARD
float delta = 0;
float movementSpeed = 600.0; // units per second
void updateDelta(double elapsedSeconds) {
    delta =  movementSpeed * elapsedSeconds;
}
double lastTimeStamp = glfwGetTime();
void renderScene(bool isShadowScene)
{
    if (isShadowScene) {
        glDisable(GL_BLEND);
    }
    else
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    // get current time
    double currentTimeStamp = glfwGetTime();
    updateDelta(currentTimeStamp - lastTimeStamp);
    lastTimeStamp = currentTimeStamp;

    // !!!
    // use delta time to change the speed of the helicopter
    blackhawkAngle += blackhawkAngleSpeed*delta;
    if (blackhawkAngle > 3600.0f)
    {
        blackhawkAngle = blackhawkAngleSpeed;
    }

    // change rendering mode
    switch (renderingMode)
    {
    case 0: glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); break;
    case 1: glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); break;
    case 2: glPolygonMode(GL_FRONT_AND_BACK, GL_POINT); break;
    default: glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); break;
    }

    glEnable(GL_CULL_FACE);
    //cull pyramids to front
    glCullFace(GL_FRONT);

    // MODEL MATRIX DEFINITION
    glm::mat4 model;

    model = glm::mat4(1.0f);

    model = glm::translate(model, glm::vec3(0.0f, 0.0f, -2.5f)); //glm::vec3(30.0f, 0.0f, -2.5f) | glm::vec3(0.0f, 0.0f, -2.5f)
    model = glm::rotate(model, -blackhawkAngle * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
    //model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));

    // use the uniform value/model. Must wrap the value in a value pointer
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

    //use the texture from here on out
    metalTexture.useTexture();
    shinyMaterial.useMaterial(uniformSpecularIntensity, uniformShininess);
    meshList[0]->renderMesh();

    //for the 2nd one
   /* model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 3.0f, -2.5f));
    model = glm::rotate(model, blackhawkAngle * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
    dirtTexture.useTexture();
    dullMaterial.useMaterial(uniformSpecularIntensity, uniformShininess);
    meshList[1]->renderMesh();*/

    glCullFace(GL_BACK);

    //for the floor
    //model = glm::mat4(1.0f);
    //model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
    //glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
    //if (ACTIVE_SKYBOX == SkyboxNames::SPACE_SCENE) moonTexture.useTexture();
    //else dirtTexture.useTexture();
    //shinyMaterial.useMaterial(uniformSpecularIntensity, uniformShininess);
    //meshList[2]->renderMesh();

    // ROVER

    model = glm::mat4(1.0f);
    model = glm::translate(model, roverPosition);
    model = glm::rotate(model, -roverAngle * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, 90.0f * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, glm::vec3(0.25f, 0.25f, 0.25f));
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
    shinyMaterial.useMaterial(uniformSpecularIntensity, uniformShininess);
    rover.renderModel();

    
    // MARS BASE

    model = glm::mat4(1.0f);
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
    groundMaterial.useMaterial(uniformSpecularIntensity, uniformShininess);
    mars_one.renderModel();

    model = glm::mat4(1.0f);
    shinyMaterial.useMaterial(uniformSpecularIntensity, uniformShininess);
    model = glm::translate(model, glm::vec3(-25.0f, 70.0f, -100.0f));
    model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));
    model = glm::rotate(model, -blackhawkAngle * toRadians * 0.5f, glm::vec3(0.0f, 1.0f, 0.0f));
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
    up_base.renderModel();

    glEnable(GL_CULL_FACE);

    

    // for our other models
    //xwing 
    if (ENABLE_XWING)
    {
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-7.0f, 0.0f, 10.0f));
        model = glm::scale(model, glm::vec3(0.006f, 0.006f, 0.006f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        shinyMaterial.useMaterial(uniformSpecularIntensity, uniformShininess);
        xwing.renderModel();
    }

    renderBlackhawk();

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(10.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
    shinyMaterial.useMaterial(uniformSpecularIntensity, uniformShininess);
    tempest.renderModel();


    // not using instanced draws
    model = glm::mat4(1.0f);
    model = glm::translate(model, moonOffset);
    model = glm::rotate(model, -blackhawkAngle * toRadians * 0.1f, glm::vec3(0.0f, 1.0f, 0.0f));
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
    moon_planet.renderModel();
    for (unsigned int i = 0; i < amount; i++)
    {
        model = modelMatricesRegular[i];
        model = glm::rotate(model, -blackhawkAngle * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));

        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        space_rock.renderModel();
    }

    // using isntanced draaws
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-260.0f, 75.0f, 0.0f));
    model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
    planet.renderModel();

    glDisable(GL_BLEND);
    rockTexture.useTexture();
    dullMaterial.useMaterial(uniformSpecularIntensity, uniformShininess);
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-260.0f, 72.5f, 0.0f));
    model = model = glm::rotate(model, 20.0f * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
    glUniform1i(shaderList[0].getInstancedDrawsLocation(), 1);
    for (unsigned int i = 0; i < rock.getMeshes().size(); i++)
    {
        std::vector<Mesh*> meshes = rock.getMeshes();
        glBindVertexArray(meshes[i]->getVAO());
        glDrawElementsInstanced(GL_TRIANGLES, static_cast<unsigned int>(meshes[i]->getIndexCount()), GL_UNSIGNED_INT, 0, amount);
        glBindVertexArray(0);
    }
    glUniform1i(shaderList[0].getInstancedDrawsLocation(), 0);
    
    // draw dome
    glEnable(GL_BLEND);
    glDisable(GL_CULL_FACE);
    model = glm::mat4(1.0f);
    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
    groundMaterial.useMaterial(uniformSpecularIntensity, uniformShininess);
    glass_dome.renderModel();

    // switch back after rendering the models
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void directionalShadowMapPass(DirectionalLight *light)
{
    directionalShadowShader.useShader();

    // setup viewport to the same size as frame buffer
    glViewport(0, 0, light->getShadowMap()->getShadowWidth(), light->getShadowMap()->getShadowHeight());
    light->getShadowMap()->write();
    //clear all the depth buffer information
    glClear(GL_DEPTH_BUFFER_BIT);

    uniformModel = directionalShadowShader.getModelLocation();
    glm::mat4 tempLightTransform = light->calculateLightTransform();
    directionalShadowShader.setDirectionalLightTransform(&tempLightTransform);

    directionalShadowShader.validate();
    renderScene(true);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
void omniShadowMapPass(PointLight* light)
{

    glViewport(0, 0, light->getShadowMap()->getShadowWidth(), light->getShadowMap()->getShadowHeight());

    omniShadowShader.useShader();
    // assign the uniforms
    uniformModel = omniShadowShader.getModelLocation();
    uniformOmniLightPos = omniShadowShader.getOmniLightPosLocation();
    uniformFarPlane = omniShadowShader.getFarPlaneLocation();

    // write to the shadow map
    light->getShadowMap()->write();

    glClear(GL_DEPTH_BUFFER_BIT);

    // position of the light in the world
    glUniform3f(uniformOmniLightPos, light->getPosition().x, light->getPosition().y, light->getPosition().z);
    glUniform1f(uniformFarPlane, light->getFarPlane());
    omniShadowShader.setOmniLightMatrices(light->calculateLightTransform());

    omniShadowShader.validate();
    renderScene(true);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

   
}
void renderPass(glm::mat4 viewMatrix, glm::mat4 projectionMatrix)
{
    // was originally beign set in our window function
   // would be cleaner to have window have a function that
   // sets the viewport
    glViewport(0, 0, 1920, 1080);

    // Clear window, rgb values between 0 and 1, aplha means transparency (first on 1.0f would make screen red)
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //clear all the colour and depth info

    
    // draw skybox
    skybox.drawSkybox(viewMatrix, projectionMatrix, teleportStrength);
    
    //go to GPU and use shader 
    shaderList[0].useShader();

    uniformModel = shaderList[0].getModelLocation();
    uniformProjection = shaderList[0].getProjectionLocation();
    uniformView = shaderList[0].getViewLocation();
    uniformEyePosition = shaderList[0].getEyePositionLocation();
    uniformSpecularIntensity = shaderList[0].getSpecularIntensityLocation();
    uniformShininess = shaderList[0].getShininessLocation();

      // uniform projection and view calculation, same for camera uniform
    glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(viewMatrix));
    glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

    // set shadows
    glUniform1i(shaderList[0].getUniformShadowMapPassEnableLocation(), ENABLE_OMNIDIRECATIONAL_SHADOW_MAP);
    // set indexed draws
    glUniform1i(shaderList[0].getInstancedDrawsLocation(), 0);
    //set fog enable
    glUniform1i(shaderList[0].getFogEnableLocation(), fogEnable);
    //set teleport strength
    glUniform1f(shaderList[0].getTeleportStrengthLocation(), teleportStrength);

    //use the light and the light transform
    shaderList[0].setDirectionalLight(&mainLight);
    shaderList[0].setPointLights(pointLights, pointLightCount, 3, 0);
    shaderList[0].setSpotLights(spotLights, spotLightCount, 3 + pointLightCount, pointLightCount);
    glm::mat4 tempLightTransform = mainLight.calculateLightTransform();
    shaderList[0].setDirectionalLightTransform(&tempLightTransform);

    // use the shadow map, with straggles on GL_TEXTURE0
    // out textures on texture unit 1 , main light on 2 
    // and the others start at texture unit 3
    mainLight.getShadowMap()->read(GL_TEXTURE2);
    shaderList[0].setTexture(1); // differentiate the textures being used
    shaderList[0].setDirectionalShadowMap(2);
    //mainLight.getShadowMap()->read(GL_TEXTURE1);
    //shaderList[0].setTexture(0);
    //shaderList[0].setDirectionalShadowMap(1);

    // set spot light to follow camera
    glm::vec3 lowerLight = camera.getCameraPosition();
    lowerLight.y -= FLASHLIGHT_OFFSET;
    spotLights[0].setFlash(lowerLight, camera.getCameraDirection());

    shaderList[0].validate();

    // draw the light cubes in the render pass
    for (int i = 0; i < pointLightCount; i++) {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, pointLights[i].getPosition());
        model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        cubes[i].renderModel();
    }

   
    //for (int i = 1; i < spotLightCount; i++) {
    //    glm::mat4 model = glm::mat4(1.0f);
    //    model = glm::translate(model, spotLights[i].getPosition());
    //    model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
    //    glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
    //    cubes[pointLightCount + i].renderModel();
    //}
       glm::mat4 model = glm::mat4(1.0f);
       model = glm::translate(model, spotLights[1].getPosition());
       model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
       glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
       cubes[6].renderModel();


    renderScene(false);
}

std::vector<glm::vec3> slicing(std::vector<glm::vec3> const& v,
    int X, int Y)
{

    // Begin and End iterator
    auto first = v.begin() + X;
    auto last = v.begin() + Y + 1;

    // Copy the element
    std::vector<glm::vec3> vector(first, last);

    // Return the results
    return vector;
}

//glm::vec3 Bezier3Points(float t, glm::vec3 P0, glm::vec3 P1, glm::vec3 P2) {
//    return (1 - t) * (P0 * (1 - t) + P1 * t) + (P1 * (1 - t) + P2 * t) * t;
//}

glm::vec3 BezierArray(float t, std::vector<glm::vec3> points) {
    if (points.size() == 2) {
        glm::vec3 P0 = points[0];
        glm::vec3 P1 = points[1];
        return P0 * (1 - t) + P1 * t;
    }
    else
    {
        std::vector<glm::vec3> vector1 = slicing(points, 0, points.size() - 2);
        std::vector<glm::vec3> vector2 = slicing(points, 1, points.size() - 1);
        return BezierArray(t, vector1) * (1 - t) + BezierArray(t, vector2) * t;
    }
}

int main()
{
    mainWindow = Window(1920, 1080); //was 800, 600 | 1366, 768
    mainWindow.initialise();
    createObjects(meshList);
    createShaders(shaderList, directionalShadowShader, omniShadowShader);


    //initialising the camera (was 1.0f sensitivity)
    camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f,0.0f,6.0f,0.2f); //25.0f test speed

    //setup the textures
    brickTexture = Texture("Textures/brick.png");
    brickTexture.loadTextureA();
    dirtTexture = Texture("Textures/dirt.png");
    dirtTexture.loadTextureA();
    plainTexture = Texture("Textures/plain.png");
    plainTexture.loadTextureA();
    rockTexture = Texture("Textures/rock/rock.png");
    rockTexture.loadTextureA();
    metalTexture = Texture("Textures/white_metal.png");
    metalTexture.loadTextureA();
    glassBlue = Texture("Textures/glass_blue.png");
    glassBlue.loadTextureA();


    //materials
    shinyMaterial = Material(4.0f, 156);
    dullMaterial = Material(0.3f, 4);
    mediumMaterial = Material(2.0f, 32);
    groundMaterial = mediumMaterial;
    
    //models - takes a long time to load on this system :(
    if (ENABLE_XWING)
    {
        xwing = Model("xwing");
        xwing.loadModel("Models/x-wing.obj");
    }

    //load blackhawk
    blackhawk_propeller_small = Model("blackhawk");
    blackhawk_propeller_small.loadModel("Models/blackhawk_propeller_small.obj");

    blackhawk_propeller_big = Model("blackhawk");
    blackhawk_propeller_big.loadModel("Models/blackhawk_propeller_big.obj");
        
    blackhawk_body = Model("blackhawk");
    blackhawk_body.loadModel("Models/blackhawk_body.obj");
  

    // load tempest
    tempest = Model("tempest");
    tempest.loadModel("Models/tempest.obj");

    planet = Model("planet");
    planet.loadModel("Models/planet.obj");

    rock = Model("rock");
    rock.loadModel("Models/rock.obj");

    rock2 = Model("rock");
    rock2.loadModel("Models/rock.obj");

    mars_one = Model("mars_one");
    mars_one.loadModel("Models/mars_one.obj");

    moon_planet = Model("moon_planet");
    moon_planet.loadModel("Models/moon_planet.obj");

    space_rock = Model("space_rock");
    space_rock.loadModel("Models/space_rock.obj");

    up_base = Model("up_base");
    up_base.loadModel("Models/up_base.obj");

    rover = Model("rover");
    rover.loadModel("Models/rover.obj");

    glass_dome = Model("glass_dome");
    glass_dome.loadModel("Models/glass_dome.obj");

    
    Model cube = Model("cube");
    cube.loadModel("Models/cubes/cube.obj");
    for (int i = 0; i < MAX_SPOT_LIGHTS + MAX_POINT_LIGHTS; i++) {
        cubes[i] = cube;
    }
    cubes[2] = cube;
    Model cube1 = Model("cube");
    cube1.loadModel("Models/cubes/cube1.obj");
    cubes[1] = cube1;
    Model cube2 = Model("cube");
    cube2.loadModel("Models/cubes/cube2.obj");
    cubes[0] = cube2;
    Model cube3 = Model("cube");
    cube3.loadModel("Models/cubes/cube3.obj");
    cubes[3] = cube3;
    Model cube3b = Model("cube");
    cube3b.loadModel("Models/cubes/cube3b.obj");
    cubes[4] = cube3b; 
    Model cube4 = Model("cube");
    cube4.loadModel("Models/cubes/cube4.obj");
    cubes[6] = cube4; // 6 beacuase 5 is skipped


    /*
    switch (ACTIVE_SKYBOX)
    {
    case SkyboxNames::ORANGE_SCENE:
    {
        // use a red tint and sun direction
        mainLight = DirectionalLight(shadowMapSizeDirectional, shadowMapSizeDirectional,
            1.0f, 0.53f, 0.3f,
            0.1f, 0.9f,
            -10.0f, -12.0f, 18.5f,
            farPlane);
        break;
    }
    case SkyboxNames::SPACE_SCENE:
        mainLight = DirectionalLight(shadowMapSizeDirectional, shadowMapSizeDirectional,   // shadow map width and height // 2048, 2048,
            1.0f, 1.0f, 1.0f, //color 1.0f, 0.53f, 0.3f for sun,  1.0f, 1.0f, 1.0f otherwise for white
            0.15f, 0.35f,  //intensity of ambient + intensity of diffuse
            0.0f, -150.0f, 130.0f, //direction was 0.0f, -15.0f, -1.0f initial
            farPlane); 
        break;
    default:
    {
        // use test light
        mainLight = DirectionalLight(shadowMapSizeDirectional, shadowMapSizeDirectional,   // shadow map width and height
            1.0f, 1.0f, 1.0f, //color 1.0f, 0.53f, 0.3f for sun,  1.0f, 1.0f, 1.0f otherwise for white
            0.125f, 0.225f,  //intensity of ambient + intensity of diffuse
            0.0f, -15.0f, -1.0f,  //direction was  0.0f, -15.0f, -10.0f to skew the shadows or 0.0f, -15.0f, -1.0f initial
            farPlane); 
    }
    }
    */

    mainLight = DirectionalLight(shadowMapSizeDirectional, shadowMapSizeDirectional,   // shadow map width and height // 2048, 2048,
        1.0f, 1.0f, 1.0f, //color 1.0f, 0.53f, 0.3f for sun,  1.0f, 1.0f, 1.0f otherwise for white
        0.15f, 0.35f,  //intensity of ambient + intensity of diffuse
        0.0f, -150.0f, 130.0f, //direction was 0.0f, -15.0f, -1.0f initial
        farPlane);

    // point lights
 

    pointLights[0] = PointLight(shadowMapSizeOthers, shadowMapSizeOthers, // shadow map size
        0.01f, farPlane, // near and far
        0.0f, 0.0f, 1.0f, // colour
        0.4f, 1.0f,  // ambient + diffuse (was 0.2, 0.6)
        9.725f, 1.0f, 0.0f,
        0.08f, 0.04f, 0.03f); //constant, linear and exponent values
    pointLightCount++;

    pointLights[2] = PointLight(shadowMapSizeOthers, shadowMapSizeOthers, // shadow map size
                                0.01f, farPlane, // near and far
                                1.0f, 0.0f, 0.0f, // colour
                                0.2f, 0.75f,  // ambient + diffuse (was 0.2, 0.6)
                                0.0f, 0.5f, -4.0f, //position, was 4 8 0 for omni , was 4.0f, 0.0f, 0.0f initial
                                0.3f, 0.2f, 0.1f); //constant, linear and exponent values
    pointLightCount++;
    
    pointLights[1] = PointLight(shadowMapSizeOthers, shadowMapSizeOthers, // shadow map size
                                0.01f, farPlane, // near and far
                                0.0f, 1.0f, 0.0f, // colour
                                0.2f, 0.75f, // ambient + diffuse (was 0.2, 0.6)
                                -4.0f, 2.0f, 0.0f, //position, was 4 8 0 for omni
                                0.3f, 0.1f, 0.1f); //constant, linear and exponent values
    pointLightCount++;

    pointLights[3] = PointLight(shadowMapSizeOthers, shadowMapSizeOthers, // shadow map size
        0.01f, farPlane, // near and far
        0.7215f, 0.0f, 0.9215f, // colour
        0.4f, 1.0f,  // ambient + diffuse (was 0.2, 0.6)
        -30.0f, 6.0f, 0.0f,
        0.75f, 0.045f, 0.0075f); //constant, linear and exponent values
    pointLightCount++;

    pointLights[4] = PointLight(shadowMapSizeOthers, shadowMapSizeOthers, // shadow map size
        0.01f, farPlane, // near and far
        0.6392f, 1.0f, 0.909f, // colour
        0.4f, 1.0f,  // ambient + diffuse (was 0.2, 0.6)
       -1.408400f, 4.280712f, 41.959518f,
        0.75f, 0.045f, 0.0075f); //constant, linear and exponent values
    pointLightCount++;


    // spot lights, first one has code to bind as flashlight to camera
    
     spotLights[0] = SpotLight(
         shadowMapSizeOthers, shadowMapSizeOthers, // shadow map size
         0.01f, farPlane, // near and far
         1.0f, 1.0f, 1.0f,
         0.0f, 1.0f,  
         0.0f, 0.0f, 0.0f, //position
         0.0f, -1.0f, 0.0f, //direction, point straight down
         0.5f, 0.0f, 0.0f, //constant, linear and exponent values
         15.0f );  //edge value of 15 degrees
    spotLightCount++;
    spotLights[0].toggle();

    spotLights[1] = SpotLight(
        shadowMapSizeOthers, shadowMapSizeOthers, // shadow map size
        0.01f, farPlane, // near and far
        0.0f, 0.9f, 1.0f, // color
        0.1f, 1.0f,
        35.8f, 1.0f, 0.75f, //position 35.8f, 1.0f, 0.75f good|   31.8f, -1.0f, 0.75f,
        0.0f, 0.0f, 0.0f, // direction
        0.1f, 0.014f,0.0007f,
        35.0f);
    spotLightCount++;
    
    // setup skybox
    setupSkybox(SkyboxNames::SPACE_SCENE, skyboxSpace);
    setupSkybox(SkyboxNames::NIGHT_SCENE, skyboxNight);
    skybox = skyboxSpace;

    // setup projection matrix: FOV in degrees, aspect ratio, near and far plane
    glm::mat4 projection = glm::perspective(glm::radians(60.0f),(GLfloat)mainWindow.getBufferWidth()
        /(GLfloat)mainWindow.getBufferHeight(),0.1f,farPlane); // far plane and near plane

    modelMatricesInstanced = generateRing(glm::vec3(0,0,0), amount);
    modelMatricesRegular = generateRing(moonOffset, amount);

    setupInstancedDraws(rock, amount, modelMatricesInstanced);

    mainWindow.getKeys()[GLFW_KEY_T] = true;
    // Loop until window closed
    while (!mainWindow.getShouldClose())
    {
        // Get and handle input event
        printFramerate();
        glfwPollEvents();
        processInput();
        
        //pass camera
        if (isAnimation)
        {
            float t = (glfwGetTime() - animationStart) / animationLength;
            if (t > 1.0f) {
                isAnimation = false;
                //camera.setYaw(55.59);
                //camera.setPitch(-17.59);
                camera.setYaw(125.189873);
                camera.setPitch(-23.189999);
            }
            else
            {
                //glm::vec3 P1 = glm::vec3(0.0f, 2.0f, -4.0f);
                //glm::vec3 P2 = glm::vec3(20.0f, 4.0f, -8.0f);
                //glm::vec3 P3 = glm::vec3(10.0f, 6.0f, -6.0f);
                //glm::vec3 P4 = glm::vec3(-10.0f, 10.0f, -15.0f);

                glm::vec3 P1 =  glm::vec3(14.078650, 15.690253, 68.466133);
                glm::vec3 P2 =  glm::vec3(35.630482, 10.943357, 48.574368);
                glm::vec3 P3 =  glm::vec3(47.994144, 14.411410, 27.318850);
                glm::vec3 P4 = glm::vec3(40.056835, 10.490824, -20.940067);
                glm::vec3 P5 = glm::vec3(12.178409, 4.900970, -10.521302);
                glm::vec3 P6 = glm::vec3(-10.244884, 2.822490, -1.930852);
                glm::vec3 P7 = glm::vec3(-8.375971, 4.274785, -22.005714);
                glm::vec3 P8 = glm::vec3(10.788021, 13.613571, -45.334103);
                glm::vec3 P9 = glm::vec3(-32.825665, 13.527882, -45.828094);
                
                glm::vec3 P10 = glm::vec3(-39.799053, 18.295841, -59.982048);
                glm::vec3 P11 = glm::vec3(-17.076637, 25.750383, -73.348541);
                glm::vec3 P12 = glm::vec3(0.081393, 34.641621, -80.219635);
                glm::vec3 P13 = glm::vec3(54.272938, 67.778526, -105.016129);
                glm::vec3 P14 = glm::vec3(72.992676, 75.406425, -111.633720);

                glm::vec3 P15 = glm::vec3(92.234703, 84.544128, -120.152641);
                glm::vec3 P16 = glm::vec3(106.776474, 94.458313, -137.874405);
                glm::vec3 P17 = glm::vec3(108.341545, 95.621765, -140.093857);
                //yaw = 125.189873; pitch = -23.189999

                std::vector<glm::vec3> point_array;
                point_array.push_back(P1);
                point_array.push_back(P2);
                point_array.push_back(P3);
                point_array.push_back(P4);
                point_array.push_back(P5);
                point_array.push_back(P6);
                point_array.push_back(P7);
                point_array.push_back(P8);
                point_array.push_back(P9);
                point_array.push_back(P10);
                point_array.push_back(P11);
                point_array.push_back(P12);
                point_array.push_back(P13);
                point_array.push_back(P14);
                point_array.push_back(P15);
                point_array.push_back(P16);
                point_array.push_back(P17);

                glm::vec3 mid = glm::vec3(-0.160640, 1.250548, 6.465411);
                glm::vec3 cameraPos = BezierArray(t, point_array);

                glm::vec3 dir = mid - cameraPos;

                camera.setCameraPosition(cameraPos);
                camera.setCameraFront(dir);
                camera.setCameraUp(glm::vec3(0, 1, 0));
            }
        }
        else
        {
            camera.keyControl(mainWindow.getKeys(), deltaTime);
            camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());
        }
        rover_move(mainWindow.getKeys(), deltaTime, 3.0f);
        rover_rotate(mainWindow.getKeys(), deltaTime, 50.0f);
        spotlight_rotate(deltaTime, 40.0f);

        
        // do the directional shadow map
       directionalShadowMapPass(&mainLight);

       // do the omnishadowmap pass for the number of existing point lights and spot lights
        for (size_t i = 0; i < pointLightCount; i++)
        {
            omniShadowMapPass(&pointLights[i]);
        }
        for (size_t i = 0; i < spotLightCount; i++)
        {
            omniShadowMapPass(&spotLights[i]);
        }
        renderPass(camera.calculateViewMatrix(), projection);

        glUseProgram(0);
        mainWindow.swapBuffers();
    }
    return 0;
}

