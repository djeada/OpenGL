#include <stdlib.h>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

using namespace std;

//Compile with g++ -o main version.cpp -lglfw -lGLEW -lGLU -lGL

const int screenWidth = 1024;
const int screenHeight = 768;

int main(int argc, char* argv[]) {

	if (!glfwInit()) 
		exit(EXIT_FAILURE);
	
	auto* window = glfwCreateWindow(screenWidth, screenHeight, "glfw", NULL, NULL);
	
	if (!window) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);
	
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
