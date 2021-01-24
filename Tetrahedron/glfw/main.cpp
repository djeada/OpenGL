#include <stdlib.h>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>
#include <GL/glut.h>

using namespace std;

//Compile with g++ -o main main.cpp -lglfw -lGLEW -lGLU -lGL
GLfloat const red[3] = {1,0,0};
GLfloat const green[3] = {0,1,0};
GLfloat const blue[3] = {0,0,1};
int screenWidth = 1024;
int screenHeight = 768;

void drawFloor() {
	glColor3f(1.0, 1.0, 1.0);
	glBegin(GL_LINES);
	for (GLfloat i = -2.5; i <= 2.5; i += 0.25) {
		glVertex3f(i, 0, 2.5); glVertex3f(i, 0, -2.5);
		glVertex3f(2.5, 0, i); glVertex3f(-2.5, 0, i);
	}
	glEnd();
}

void drawCube(GLfloat (&n)[6][3], GLfloat (&v)[8][3], GLint (&faces)[6][4]) {
drawFloor();
	glBegin(GL_TRIANGLE_STRIP);
	glColor3f(1, 1, 1); glVertex3f(0, 2, 0);
	glColor3f(1, 0, 0); glVertex3f(-1, 0, 1);
	glColor3f(0, 1, 0); glVertex3f(1, 0, 1);
	glColor3f(0, 0, 1); glVertex3f(0, 0, -1.4);
	glColor3f(1, 1, 1); glVertex3f(0, 2, 0);
	glColor3f(1, 0, 0); glVertex3f(-1, 0, 1);
	glEnd();
}

void initCube() {

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-2, 2, -1.5, 1.5, 1, 40);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0, 0, -3);
	glRotatef(50, 1, 0, 0);
	glRotatef(70, 0, 1, 0);
}

int main(int argc, char* argv[]) {

	if (!glfwInit()) 
		exit(EXIT_FAILURE);

	auto* window = glfwCreateWindow(screenWidth, screenHeight, "triangles", NULL, NULL);
	
	if (!window) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);
  	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	
	GLfloat n[6][3] = {{-1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {1.0, 0.0, 0.0},
		{0.0, -1.0, 0.0}, {0.0, 0.0, 1.0}, {0.0, 0.0, -1.0} };

	GLint faces[6][4] = { {0, 1, 2, 3}, {3, 2, 6, 7}, {7, 6, 5, 4}, 
		{4, 5, 1, 0}, {5, 6, 2, 1}, {7, 4, 0, 3} };
	
	GLfloat v[8][3] = {	{-1, -1, 1}, {-1, -1, -1}, {-1, 1, -1}, {-1, 1, 1}, 
		{1, -1, 1}, {1, -1, -1}, {1, 1, -1}, {1, 1, 1}}; 

	initCube();

	while (!glfwWindowShouldClose(window)) {
		glfwGetFramebufferSize(window, &screenWidth, &screenHeight);
		glViewport(0, 0, screenWidth, screenHeight);
		glClearColor(0, 0.5, 0.5, 1);
  		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		drawCube(n,v,faces);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}	

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;	

}
