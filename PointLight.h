#pragma once
#include "Light.h"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <vector>
#include "OmniShadowMap.h"
// a light that comes from a point in the world, has an actual
// relevant distance to the other objects, and an attenuation depending
// on that distance
class PointLight :
	public Light
{
public:
	PointLight();
	// added the shadow and the near/far planes 
	// because the projection needs to be calculated
	PointLight(GLuint shadowWidth, GLuint shadowHeight,
		GLfloat near, GLfloat far,
		GLfloat red, GLfloat green, GLfloat blue,
		GLfloat aIntensity, GLfloat dIntensity,
		GLfloat xPos, GLfloat yPos, GLfloat zPos,
		GLfloat con, GLfloat lin, GLfloat exp);

	void useLight(GLuint ambientIntensityLocation, GLuint ambientColourLocation,
		GLuint diffuseIntensityLocation, GLuint positionLocation,
		GLuint constantLocation, GLuint linearLocation, GLuint exponentLocation);

	// vector or perspective projection transforms
	std::vector<glm::mat4> calculateLightTransform();

	glm::vec3 getPosition();
	GLfloat getFarPlane();


	~PointLight();

protected:
	// position of our light (exact location in the world
	// unlike the direction of the directional light)
	glm::vec3 position;

	// values to calculate the attenuation of our point light
	// (the attenuation) in quadratic
	GLfloat constant, linear, exponent;
	// where the far plane is
	GLfloat farPlane;
};



