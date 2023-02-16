#pragma once
#include<GL/glew.h>


class Texture
{
public:
	Texture();
	Texture(const char* fileloc);
	~Texture();

	bool loadTexture();
	bool loadTextureA();
	void useTexture();
	void clearTexture();
private:
	// id of the loaded texture
	GLuint textureID;
	int width, height, bitDepth;

	const char* fileLocation;
};

