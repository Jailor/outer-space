#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Mesh.h"
#include <vector>
#include <glm/ext/vector_float3.hpp>
#include <glm/geometric.hpp>
#include "CommonValues.h"
#include "Shader.h"
#include "Model.h"
#include "Skybox.h"

// Utility function to calculate the normals through interpolation
// Achieves a smooth effect for shadow calculations on something like a sphere for example.
void calcAverageNormals(unsigned int* indices, unsigned int indiceCount,
    GLfloat* vertices, unsigned int verticeCount,
    unsigned int vLength, unsigned int normalOffset);
// Initialises the objects defined manually by vertices.
// Currently defines the 2 pyramids and the floor of the world.
// TO FIX: Uses the ENABLE_SKYBOX flag for dynamically choosing the size of the floor
void createObjects(std::vector<Mesh*>& meshList);

// Initialises the shaders necessary for the program operation (except for the Skybox shader, which is handled
// by the Skybox class). Uses the constant values defined in the common values shader and the methods defined in
// the shader class to define the shaders using file locations.
void createShaders(std::vector<Shader> &shaderList, Shader &directionalShadowShader, Shader &omniShadowShader);

// Generates a ring of rocks around a point centered at translate vector.
// Amount gives the number of rocks to generate. 
// Return : a mat4 array where the rock positions are stored. 
glm::mat4* generateRing(glm::vec3 translateVec, unsigned int amount);

// sets up the buffer and the VAO/ VBO for instanced drawing
void setupInstancedDraws(Model rock, unsigned int amount, glm::mat4* modelMatricesInstanced);

// Loads the skybox of the specified type
void setupSkybox(SkyboxNames scene_enum, Skybox& skybox);