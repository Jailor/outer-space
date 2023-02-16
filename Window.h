#pragma once

#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Window
{
public:
	Window();
	Window(GLint windowWidth, GLint windowHeight);
	int initialise();
	GLint getBufferWidth() { return bufferWidth;}
	GLint getBufferHeight() { return bufferHeight; }
	bool getShouldClose() { return glfwWindowShouldClose(mainWindow); }
	void swapBuffers() {
		// Two scenes, one that you are drawing to and one that can be seen, 
		// sometimes with 3 if you use triple buffering. This switch will switch them
		// and show the prepared scene.
		glfwSwapBuffers(mainWindow);
	}
	bool* getKeys() { return keys; }
	GLfloat getXChange();
	GLfloat getYChange();
	~Window();
private:
	GLFWwindow* mainWindow;

	GLint width, height;
	GLint bufferWidth, bufferHeight;

	// array of key presses
	bool keys[1024];

	// for mouse coordinates
	GLfloat lastX;
	GLfloat lastY;
	GLfloat xChange;
	GLfloat yChange;
	bool mouseFirstMoved;

	//GFLW function needs a static function for callback
	static void handleKeys(GLFWwindow* window, int key, int code, int action, int mode);
	
	//create the necessary callbacks
	void createCallbacks();
	// function to handle the mouse change
	static void handleMouse(GLFWwindow* window, double xPos, double yPos);
};

