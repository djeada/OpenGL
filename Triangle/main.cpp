#include <stdlib.h>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

using namespace std;

//Compile with g++ -o main version.cpp -lglfw -lGLEW -lGLU -lGL

void draw() {
	glBegin(GL_TRIANGLES);
	glColor3f(1,0,0);
	glVertex3f(-1,0,0);
	glColor3f(0,1,0);
	glVertex3f(0,1,0);
	glColor3f(0,0,1);
	glVertex3f(1,0,0);
	glEnd();
}

int main(int argc, char* argv[]) {

	if (!glfwInit()) 
		exit(EXIT_FAILURE);

	int screenWidth = 1024;
	int screenHeight = 768;
	
	auto* window = glfwCreateWindow(screenWidth, screenHeight, "glfw", NULL, NULL);
	
	if (!window) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);
	while (!glfwWindowShouldClose(window)) {
		glfwGetFramebufferSize(window, &screenWidth, &screenHeight);
		glViewport(0,0,screenWidth, screenHeight);
		glClearColor(0,0.5,0.5,1);
		glClear(GL_COLOR_BUFFER_BIT);

		draw();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}	


	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;	

}
