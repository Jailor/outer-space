#pragma once

#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <string>

#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/glm.hpp>
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"

#include "CommonValues.h"

class Shader
{
public:
	Shader();
	~Shader();
	void createFromString(const char* vertexCode, const char* fragmentCode);
	void createFromFiles(const char* vertexLocation, const char* fragmentLocation);
	// create with gemetry shader
	void createFromFiles(const char* vertexLocation, const char* geometryLocation, const char* fragmentLocation);

	void validate();

	std::string readFile(const char* fileLocation);

	GLuint getProjectionLocation();
	GLuint getModelLocation();
	GLuint getViewLocation();
	GLuint getAmbientIntensityLocation();
	GLuint getAmbientColourLocation();
	GLuint getDiffuseIntensityLocation();
	GLuint getDirectionLocation();
	GLuint getSpecularIntensityLocation();
	GLuint getShininessLocation();
	GLuint getEyePositionLocation();
	GLuint getOmniLightPosLocation();
	GLuint getFarPlaneLocation();
	GLuint getUniformShadowMapPassEnableLocation();
	GLuint getInstancedDrawsLocation();
	GLuint getFogEnableLocation();
	GLuint getTeleportStrengthLocation();

	// reduce the amount of code by having the shader
	// itself handle the light
	void setDirectionalLight(DirectionalLight* dLight);
	void setPointLights(PointLight* pLight, unsigned int lightCount, unsigned int textureUnit, unsigned int offset);
	void setSpotLights(SpotLight* sLight, unsigned int lightCount, unsigned int textureUnit, unsigned int offset);
	
	// allow for changing the texture
	void setTexture(GLuint textureUnit);
	void setDirectionalShadowMap(GLuint textureUnit);
	void setDirectionalLightTransform(glm::mat4* lTransform);
	// set omnidirectional shadow map light projection matrices
	void setOmniLightMatrices(std::vector<glm::mat4> lightMatrices);

	void useShader();
	void clearShader();
private:
	// number of point lights
	int pointLightCount;
	// number of spot lights in the scene
	int spotLightCount;

	GLuint shaderID, uniformProjection, uniformModel, uniformView ,uniformEyePosition;
	//uniform values for the light
	GLuint  uniformSpecularIntensity, uniformShininess;
	// shadow uniforms
	GLuint uniformTexture, uniformDirectionalLightTransform, uniformDirectionalShadowMap;
	// uniforms for the omindirectional shadow map
	GLuint uniformOmniLightPos, uniformFarPlane;
	// light projection matrices to pass those 6 matrices
	GLuint uniformLightMatrices[6];
	// for enabling the shadow map
	GLuint uniformShadowMapPassEnable;
	// for enabling instanced draws
	GLuint uniformInstancedDraws;
	// for enabling and disabling the fog
	GLuint uniformFogEnable;
	// gives the strength of the teleport effect
	GLuint uniformTeleportStrength;

	// struct that holds uniforms for a directional light
	struct {
		GLuint uniformColour;
		GLuint uniformAmbientIntensity;
		GLuint uniformDiffuseIntensity;

		GLuint uniformDirection;
	}uniformDirectionalLight;

	

	GLuint uniformPointLightCount;
	// struct that holds uniforms for a point light
	struct {
		GLuint uniformColour;
		GLuint uniformAmbientIntensity;
		GLuint uniformDiffuseIntensity;

		// point light specific
		GLuint uniformPosition;
		GLuint uniformConstant;
		GLuint uniformLinear;
		GLuint uniformExponent;
	}uniformPointLight[MAX_POINT_LIGHTS];


	
	GLuint uniformSpotLightCount;
	// struct to hold information of spot lights
	struct {
		GLuint uniformColour;
		GLuint uniformAmbientIntensity;
		GLuint uniformDiffuseIntensity;

		GLuint uniformPosition;
		GLuint uniformConstant;
		GLuint uniformLinear;
		GLuint uniformExponent;

		// spot light specific
		GLuint uniformDirection;
		GLuint uniformEdge;
	} uniformSpotLight[MAX_SPOT_LIGHTS];

	// uniform for the omni shadow map, in total as many as point lights and spot lights
	struct {
		GLuint uniformShadowMap;
		GLuint uniformFarPlane;
	} uniformOmniShadowMap[MAX_POINT_LIGHTS + MAX_SPOT_LIGHTS];

	void compileShader(const char* vertexCode, const char* fragmentCode);
	void compileShader(const char* vertexCode, const char* geometryCode, const char* fragmentCode);
	//adds shader to the specified gpu program
	void addShader(GLuint theProgram, const char* shaderCode, GLenum shaderType);
	void compileProgram();
};

