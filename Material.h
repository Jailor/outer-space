#pragma once

#include <GL/glew.h>

class Material
{
public:
	Material();
	Material(GLfloat sIntensity, GLfloat shine);

	void useMaterial(GLuint specularIntensityLocation,
		GLuint shininessLocation);
	~Material();

private:
	// how much specular light should be on the object
	GLfloat specularIntensity;
	// how *shiny* our object is, bigger means bigger concetration into one region 
	GLfloat shininess;
};

