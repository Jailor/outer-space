#pragma once
#include "ShadowMap.h"
// a shadow map that uses a point light or a spot light instead of a simple directional light
// uses perspective projection instead of the ortographic projection
// Uses the shadow map as a base class
class OmniShadowMap :
	public ShadowMap
{
public:
	OmniShadowMap();

	bool init(unsigned int width, unsigned int height);

	void write();

	void read(GLenum TextureUnit);

	~OmniShadowMap();
};

