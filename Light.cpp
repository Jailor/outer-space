#include "Light.h"

//general light class, has colour and intensity for ambient and diffuse
Light::Light(GLuint shadowWidth, GLuint shadowHeight, GLfloat red, GLfloat green, GLfloat blue, GLfloat ambIntenisty,GLfloat dIntensity)
{
	colour = glm::vec3(red,green,blue);
	ambientIntensity = ambIntenisty;

	diffuseIntensity = dIntensity;

	shadowMap = new ShadowMap();
	// initialise the shadow map
	shadowMap->init(shadowWidth, shadowHeight);

	lightProj = glm::mat4(1.0f);
}


Light::Light()
{
	// white light, how much of a colour should be shown
	// a red light should only show the red parts of object
	colour = glm::vec3(1.0f, 1.0f, 1.0f);
	ambientIntensity = 1.0f;
	//arrow pointing straight down
	//direction = glm::vec3(0.0f, -1.0f, 0.0f);
	diffuseIntensity = 0.0f;
	//shadowMap = nullptr;
	//lightProj = glm::mat4(1.0f);
}



Light::~Light()
{

}

//void Light::useLight(GLuint ambientIntensityLocation, GLuint colourLocation,
//	GLuint diffuseIntensityLocation)
//{
//	// create uniform variable for the light
//	glUniform3f(colourLocation, colour.x, colour.y, colour.z);
//	glUniform1f(ambientIntensityLocation, ambientIntensity);
//
//	//create uniform for diffuse
//	glUniform1f(diffuseIntensityLocation, diffuseIntensity);
//}
