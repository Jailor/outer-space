#pragma once

#include <glm/glm.hpp>
#include <GL/glew.h>

#include "ShadowMap.h"

class Light
{
public:
	Light();
	// now has a shadow
	Light(GLuint shadowWidth, GLuint shadowHeight,
		GLfloat red, GLfloat green, GLfloat blue, GLfloat ambIntenisty, GLfloat dIntensity);
	ShadowMap* getShadowMap() { return shadowMap; }
	~Light();
protected:
	// colour of the light
	glm::vec3 colour;
	// intensity of the ambient light
	GLfloat ambientIntensity;

	//variables for diffused lighting
	//glm::vec3 direction;
	GLfloat diffuseIntensity;

	// projection matrix
	glm::mat4 lightProj;

	ShadowMap* shadowMap;
};

