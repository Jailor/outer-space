#pragma once
#include "PointLight.h"
class SpotLight :
	public PointLight
{
public:
	SpotLight();

	SpotLight(GLuint shadowWidth, GLuint shadowHeight,
		GLfloat near, GLfloat far,
		GLfloat red, GLfloat green, GLfloat blue,
		GLfloat aIntensity, GLfloat dIntensity,
		GLfloat xPos, GLfloat yPos, GLfloat zPos,
		GLfloat xDir, GLfloat yDir, GLfloat zDir,
		GLfloat con, GLfloat lin, GLfloat exp,
		GLfloat edg);

	void useLight(GLuint ambientIntensityLocation, GLuint ambientColourLocation,
		GLuint diffuseIntensityLocation, GLuint positionLocation, GLuint directionLocation,
		GLuint constantLocation, GLuint linearLocation, GLuint exponentLocation,
		GLuint edgeLocation);

	void toggle() { isOn = !isOn; }

	// Take a position and a direction and make the torch
	// point at a location. Can pass camera values
	void setFlash(glm::vec3 pos, glm::vec3 dir);

	void setDirection(glm::vec3 dir);

	~SpotLight();

private:
	// the direction the spotlight is facing in
	glm::vec3 direction;
	// edge and processed edge: represents the angle of the light,
	// how much of 360° it covers
	GLfloat edge, procEdge;

	bool isOn;
};



