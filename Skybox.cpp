#include "Skybox.h"



Skybox::Skybox()
{
	skyMesh = nullptr;
	skyShader = nullptr;
	textureId = -1;
	uniformProjection = -1;
	uniformView = -1;
}

Skybox::Skybox(std::vector<std::string> faceLocations) : Skybox()
{
	// Shader Setup
	skyShader = new Shader();
	skyShader->createFromFiles(skyboxVertexShader, skyboxFragmentShader);

	uniformProjection = skyShader->getProjectionLocation();
	uniformView = skyShader->getViewLocation();


	// Texture Setup
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);

	int width, height, bitDepth;

	for (size_t i = 0; i < 6; i++)
	{
		unsigned char* texData = stbi_load(faceLocations[i].c_str(), &width, &height, &bitDepth, STBI_rgb);
		if (!texData)
		{
			printf("Failed to find: %s\n", faceLocations[i].c_str());
			return;
		}
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // fix, same for stbi enum,  let's hope this works...
		//glPixelStorei(GL_PACK_ALIGNMENT, 1); 

		// because we are treating the cube map as a pile of 2D textures,
		// we can use glTextImage2D for this. Uses hardcoded GL_RGB.
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, texData);
		stbi_image_free(texData);
	}

	// clamp to edge and GL_LINEAR behaviour
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	// Mesh Setup
	unsigned int skyboxIndices[] = {
		// front
		0, 1, 2,
		2, 1, 3,
		// right
		2, 3, 5,
		5, 3, 7,
		// back
		5, 7, 4,
		4, 7, 6,
		// left
		4, 6, 0,
		0, 6, 1,
		// top
		4, 0, 5,
		5, 0, 2,
		// bottom
		1, 6, 3,
		3, 6, 7
	};

	// we don't need to set any uv or normals values because of no lighting
	float skyboxVertices[] = {
		-1.0f, 1.0f, -1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		-1.0f, -1.0f, -1.0f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		1.0f, 1.0f, -1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		1.0f, -1.0f, -1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,

		-1.0f, 1.0f, 1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		-1.0f, -1.0f, 1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		1.0f, -1.0f, 1.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f
	};
	const int indicesLength = sizeof(skyboxIndices) / sizeof(skyboxIndices[0]);
	const int verticesLength = sizeof(skyboxVertices) / sizeof(skyboxVertices[0]);

	skyMesh = new Mesh();
	skyMesh->createMesh(skyboxVertices, skyboxIndices, verticesLength, indicesLength);
}

void Skybox::drawSkybox(glm::mat4 viewMatrix, glm::mat4 projectionMatrix, float teleportStrength)
{
	// Make sure that we aren't using the depth mask
	// If we don't do this, we won't see anything beyond the skybox.
	// We also want to get rid of any translations that happen, which are on the last
	// columns of the view matrix. (but we do want to keep scaling and rotate)
	viewMatrix = glm::mat4(glm::mat3(viewMatrix));

	glDepthMask(GL_FALSE);

	skyShader->useShader();

	glUniform1f(skyShader->getTeleportStrengthLocation(), teleportStrength);

	glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(viewMatrix));

	// we can use gl texture 0 in this case because this shader will
	// only use this shader with only one sampler cube and no other sampler 2Ds
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);

	skyShader->validate();

	skyMesh->renderMesh();

	glDepthMask(GL_TRUE);
}

Skybox::~Skybox()
{
}

