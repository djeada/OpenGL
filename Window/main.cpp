#include <stdlib.h>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

using namespace std;

//Compile with g++ -o main version.cpp -lglfw -lGLEW -lGLU -lGL

const int screenWidth = 1024;
const int screenHeight = 800;

void glfw_onKey(GLFWwindow* window, int key, int scancode, int action, int mode) {
	cout << "Key pressed: " << key << endl;

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

int main(int argc, char* argv[]) {

	// Intialize GLFW 
	if (!glfwInit()) 
		exit(EXIT_FAILURE);
	
	auto* window = glfwCreateWindow(screenWidth, screenHeight, "glfw", NULL, NULL);
	
	if (!window) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);
		
	glfwSetKeyCallback(window, glfw_onKey);

	while (!glfwWindowShouldClose(window)) {
		glViewport(0,0,screenWidth, screenHeight);
		glClearColor(0,0.5,0.5,1);
		glClear(GL_COLOR_BUFFER_BIT);

		glfwSwapBuffers(window);
	
		//listen for window events
		glfwPollEvents();
	}	


	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;	

}
