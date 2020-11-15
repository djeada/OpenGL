#include <stdlib.h>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

using namespace std;

//Compile with g++ -o main version.cpp -lglfw -lGLEW -lGLU -lGL
GLfloat const red[3] = {1,0,0};
GLfloat const green[3] = {0,1,0};
GLfloat const blue[3] = {0,0,1};

void drawTriangle() {
	glBegin(GL_TRIANGLES);
	glColor3fv(red);
	glVertex3f(-1,-1,0);
	glColor3fv(green);
	glVertex3f(0,1,0);
	glColor3fv(blue);
	glVertex3f(1,-1,0);
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
		glViewport(0, 0, screenWidth, screenHeight);
		glClearColor(0, 0.5, 0.5, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		drawTriangle();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}	


	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;	

}
