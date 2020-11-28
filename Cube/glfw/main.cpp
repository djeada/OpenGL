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

void drawCube(GLfloat (&n)[6][3], GLfloat (&v)[8][3], GLint (&faces)[6][4]) {

	for (int i = 0; i < 6; i++) {
		glBegin(GL_QUADS);
		glNormal3fv(&n[i][0]);
		glVertex3fv(&v[faces[i][0]][0]);
		glVertex3fv(&v[faces[i][1]][0]);
		glVertex3fv(&v[faces[i][2]][0]);
		glVertex3fv(&v[faces[i][3]][0]);
		glEnd();
	}
}

void initCube() {

	glLightfv(GL_LIGHT0, GL_DIFFUSE, green);
	GLfloat light_position[] = {1.0, 1.0, 1.0, 0.0}; 
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);

	///Setup the view of the cube. */
	glMatrixMode(GL_PROJECTION);
	gluPerspective(40.0, 1.0, 1.0, 10.0);
	glMatrixMode(GL_MODELVIEW);
	gluLookAt(0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.);     
	glTranslatef(0.0, 0.0, -1.0);
	glRotatef(60, 1.0, 0.0, 0.0);
	glRotatef(-20, 0.0, 0.0, 1.0);
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
