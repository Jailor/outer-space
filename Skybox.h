#pragma once

#include <vector>
#include <string>

#include <GL\glew.h>

#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>

#include "CommonValues.h"

#include "Mesh.h"
#include "Shader.h"

// Creates the image of a larger world around us. 
// It is a cube map texture, a 1x1x1 cube around the camera
// and when it is drawn it does no depth culling/testing ( the skybox is
// thus drawn at infinity), and every "real" object will appear
// in front of the skybox. 
class Skybox
{
public:
	Skybox();

	Skybox(std::vector<std::string> faceLocations);
	// No need for a model matrix, the skybox will always be 
	// where the camera is.
	void drawSkybox(glm::mat4 viewMatrix, glm::mat4 projectionMatrix, float teleportStrength);

	~Skybox();

private:
	Mesh* skyMesh;
	// skybox will have its own shader
	Shader* skyShader;
	// could improve by creating a cube texture class to handle this
	GLuint textureId;
	GLuint uniformProjection, uniformView;
};


