#pragma once

#include<GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

class Camera
{
public:
	Camera();
	Camera(glm::vec3 startPosition, glm::vec3 startUp, GLfloat startYaw, GLfloat startPitch ,GLfloat startMoveSpeed, GLfloat startTurnSpeed);
	void keyControl(bool* keys, GLfloat deltaTime);
	void mouseControl(GLfloat xChange, GLfloat yChange);
	glm::mat4 calculateViewMatrix();

	glm::vec3 getCameraPosition();
	glm::vec3 getCameraDirection();
	
	GLfloat getYaw();
	GLfloat getPitch();

	void setYaw(GLfloat yaw);
	void setPitch(GLfloat pitch);

	void setCameraPosition(glm::vec3 newPosition);
	void setCameraFront(glm::vec3 newFront);
	void setCameraUp(glm::vec3 newUp);
	~Camera();

private:
	glm::vec3 position;  
	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 worldUp; //sky, directly above

	// left to right
	GLfloat yaw; 
	// up and down
	GLfloat pitch; 

	// how fast the camera is moving
	GLfloat moveSpeed;
	GLfloat turnSpeed;

	void update();
};

