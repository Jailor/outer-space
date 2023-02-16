#pragma once

#include <stdio.h>

#include <GL\glew.h>

class ShadowMap
{
public:
	ShadowMap();

	// Initialize the shadow map
	virtual bool init(unsigned int width, unsigned int height);
	// Write to our shadow map when doing the first pass,
	// bind the FBO to the shadow map object
	virtual void write();
	// Allows to read the shadow map as a texture 
	virtual void read(GLenum TextureUnit);

	GLuint getShadowWidth() { return shadowWidth; }
	GLuint getShadowHeight() { return shadowHeight; }

	~ShadowMap();
protected:
	// IDs for the frame buffer object and the shadow map itself
	GLuint FBO, shadowMap;
	// We need these for the shadow render. Want to make sure
	// that the viewport is the same size as our shadow map
	GLuint shadowWidth, shadowHeight;
};
