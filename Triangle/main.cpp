#include <stdlib.h>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>

using namespace std;

//Compile with g++ -o main version.cpp -lglfw -lGLEW -lGLU -lGL
GLfloat const red[3] = {1,0,0};
GLfloat const green[3] = {0,1,0};
GLfloat const blue[3] = {0,0,1};
int screenWidth = 1024;
int screenHeight = 768;
	
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

void draw() {
	static float counter = 0.0;

	counter += 0.1;

	//Orthogonal Porjection Matrix transform
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-(float) screenWidth /screenHeight, (float) screenWidth /screenHeight, -1.f, 1.f, 1.f, -1.f);

	//ModelView Matrix transform
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	for (int i = 0; i < 3; i++) {
		float t = (float)i/10;
		glPushMatrix();

		glTranslatef(t*sin(counter), 0,0);
		glRotatef(360*t*counter, 0, 0, 1);
		glScalef(1-t,1-t,1-t);

		drawTriangle();

		glPopMatrix();
	}
}

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
		glfwGetFramebufferSize(window, &screenWidth, &screenHeight);
		glViewport(0, 0, screenWidth, screenHeight);
		glClearColor(0, 0.5, 0.5, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		draw();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}	

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;	

}
