#include "ShadowMap.h"



ShadowMap::ShadowMap()
{
	FBO = 0;
	shadowMap = 0;
	shadowWidth = -1;
	shadowHeight = -1;
}

bool ShadowMap::init(unsigned int width, unsigned int height)
{
	shadowWidth = width; shadowHeight = height;

	// generate a frame buffer (a screen)
	glGenFramebuffers(1, &FBO);
	// generate texture for shadow map and bind it
	glGenTextures(1, &shadowMap);
	glBindTexture(GL_TEXTURE_2D, shadowMap);
	// use the depth component (single value) instead of RGB or RGBA
	// represented in the normalized device coordinates. Depth is
	// represented from 0 to 1, where one is close and the other one is far.
	// Mapping to device coordinates using change of scale with 0.5.
	// Nullptr is used to have the map empty. Will be written to later.
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //for less rounding, use GL_NEAREST instead of GL_LINEAR
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// not using GL_REPEAT is advised because we want to clamp it to the border
	// Not doing it causes a problem where we cast shadows inappropriate places
	// https://learnopengl.com/Advanced-Lighting/Shadows/Shadow-Mapping to learn more
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	// border definition, set the border to be for the shadow to be non existent
	// (so 1.0f), so whatever the light will see, it will be plain light beyond it
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);
	// connect framebuffer to texture, GL_DEPTH_ATTACHMENT will use depth data
	glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap, 0);

	// we only care about the depth data, we don't read or draw to the color attachment
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	// error checking
	GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (Status != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("Framebuffer error in shadow map: %d\n", Status);
		return false;
	}
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return true;
}

void ShadowMap::write()
{
	// instead of drawing to the default frame buffer,
	// it will draw to the off screen one
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);
}

void ShadowMap::read(GLenum texUnit)
{
	// when we want to read from it, have it as an active texture
	glActiveTexture(texUnit);
	glBindTexture(GL_TEXTURE_2D, shadowMap);
}

ShadowMap::~ShadowMap()
{
	if (FBO)
	{
		glDeleteFramebuffers(1, &FBO);
	}

	if (shadowMap)
	{
		glDeleteTextures(1, &shadowMap);
	}
}

