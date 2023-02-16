#include "Texture.h"
#include <stdio.h>
#include "stb_image.h"


Texture::Texture() 
{
	textureID = 0;
	width = 0;
	height = 0;
	bitDepth = 0;
	fileLocation = "";
}
Texture::Texture(const char* fileloc):Texture()
{
	fileLocation = fileloc;
}

bool Texture::loadTexture()
{
	//load image using STB
	unsigned char* textData = stbi_load(fileLocation,&width,&height,&bitDepth, STBI_rgb);
	if (!textData)
	{
		printf("Failed to find %s !\n", fileLocation);
		return false;
	}
	//printf("Channels from loadTexture: %d !\n", bitDepth);
	// generate a texture and store the ID, similar to VAO/vbo
	glGenTextures(1, &textureID);
	//bind a 2D texture (most common) to the id
	glBindTexture(GL_TEXTURE_2D, textureID);

	// define the behaviour if you leave the border: repeat, invert, extend last pixels/clamp, etc.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// behavior when you zoom: liniar for blurry, gl_nearest would look
	// look give a more pixelated look by choosing the nearest pixel
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// type; mipmap level with smaller and smaller textures for a nicer effect when zooming in/out;
	// image format: RGB or RGB_A; width; height; legacy; format of data colour; data type; data location
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // fix, same as stbi parameter, let's hope this works...
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, textData);
	//automatically generate mipmap
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	// free the data
	stbi_image_free(textData);

	return true;
}

bool Texture::loadTextureA()
{
	//printf("Channels from loadTextureA: %d !\n", bitDepth);
	//load image using STB
	unsigned char* textData = stbi_load(fileLocation, &width, &height, &bitDepth, STBI_rgb_alpha);
	if (!textData)
	{
		printf("Failed to find %s !\n", fileLocation);
		return false;
	}
	// generate a texture and store the ID, similar to VAO/vbo
	glGenTextures(1, &textureID);
	//bind a 2D texture (most common) to the id
	glBindTexture(GL_TEXTURE_2D, textureID);

	// define the behaviour if you leave the border: repeat, invert, extend last pixels/clamp, etc.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// behavior when you zoom: liniar for blurry, gl_nearest would look
	// look give a more pixelated look by choosing the nearest pixel
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// type; mipmap level with smaller and smaller textures for a nicer effect when zooming in/out;
	// image format: RGB or RGB_A; width; height; legacy; format of data colour; data type; data location
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, textData);
	//automatically generate mipmap
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	// free the data
	stbi_image_free(textData);

	return true;
}

void Texture::useTexture()
{
	// generate texture unit 1  (could use multiple textures and bind to texture1 etc,)
	// modern graphics cards usually have a minimum of 16 texture mapping units
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textureID);
}

void Texture::clearTexture()
{
	glDeleteTextures(1, &textureID);
	textureID = 0;
	width = 0;
	height = 0;
	bitDepth = 0;
	fileLocation = "";
}
Texture::~Texture()
{

}
