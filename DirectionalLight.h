#pragma once
#include "Light.h"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
// inherits from light, has all the above plus a direction where the light is coming from
// this direction is just that, from infinity a bunch of parallel lines, sort of like our sun
class DirectionalLight :
	public Light
{
public:
	DirectionalLight();
	// light that has a direction
	DirectionalLight(GLuint shadowWidth, GLuint shadowHeight,
		GLfloat red, GLfloat green, GLfloat blue,
		GLfloat aIntensity, GLfloat dIntensity,
		GLfloat xDir, GLfloat yDir, GLfloat zDir, float farPlane);

	void useLight(GLuint ambientIntensityLocation, GLuint ambientColourLocation,
		GLuint diffuseIntensityLocation, GLuint directionLocation);

	void setAmbientDiffuse(glm::vec2 values);
	void setDirection(glm::vec3 dir);

	glm::mat4 calculateLightTransform();
	~DirectionalLight();

private:
	glm::vec3 direction;
};


