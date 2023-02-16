#include "Camera.h"

Camera::Camera():Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f),0.0f,0.0f,0.0f,0.0f)
{}
Camera::Camera(glm::vec3 startPosition, glm::vec3 startUp, GLfloat startYaw, GLfloat startPitch, GLfloat startMoveSpeed, GLfloat startTurnSpeed)
{
	position = startPosition;
	worldUp = startUp;
	yaw = startYaw;
	pitch = startPitch;
	front = glm::vec3(0.0f, 0.0f, -1.0f);
	moveSpeed = startMoveSpeed;
	turnSpeed = startTurnSpeed;

	update();
}

// updates the camera values with MATH
void Camera::update()
{
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	front = glm::normalize(front);

	// calculate right using the cross product
	right = glm::normalize(glm::cross(front, worldUp));
	up = glm::normalize(glm::cross(right, front));
}

void Camera::keyControl(bool* keys, GLfloat deltaTime)
{
	GLfloat velocity = moveSpeed * deltaTime;
	if (keys[GLFW_KEY_W]) position += front * velocity;
	if (keys[GLFW_KEY_A]) position -= right * velocity;
	if (keys[GLFW_KEY_S]) position -= front * velocity;
	if (keys[GLFW_KEY_D]) position += right * velocity;
}

void Camera::mouseControl(GLfloat xChange, GLfloat yChange)
{
	xChange *= turnSpeed;
	yChange *= turnSpeed;

	yaw += xChange;
	pitch += yChange;

	//limit pitch so we don't roll over
	if (pitch > 89.0f)
	{
		pitch = 89.0f;
	}

	if (pitch < -89.0f)
	{
		pitch = -89.0f;
	}

	update();
}

glm::mat4 Camera::calculateViewMatrix()
{
	return glm::lookAt(position, position + front, up);
}

glm::vec3 Camera::getCameraPosition()
{
	return position;
}
glm::vec3 Camera::getCameraDirection()
{
	return glm::normalize(front);
}
GLfloat Camera::getYaw()
{
	return yaw;
}
GLfloat Camera::getPitch()
{
	return pitch;
}
void Camera::setYaw(GLfloat yaw)
{
	this->yaw = yaw;
}
void Camera::setPitch(GLfloat pitch)
{
	this->pitch = pitch;
}
void Camera::setCameraPosition(glm::vec3 newPosition)
{
	position = newPosition;
}
void Camera::setCameraFront(glm::vec3 newFront)
{
	newFront = glm::normalize(newFront);
	this->front = newFront;
}
void Camera::setCameraUp(glm::vec3 newUp)
{
	this->up = newUp;
}
Camera::~Camera()
{

}