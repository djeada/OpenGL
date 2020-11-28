#include <iostream>
#define GLEW_STATIC
#include <GL/glew.h>
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include "vertex_buffer.h"
#include "shader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "libs/stb_image.h"

//export MESA_GL_VERSION_OVERRIDE=3.3
//compile with g++ -o main main.cpp vertex_buffer.cpp shader.cpp -lSDL2main -lSDL2 -lGLEW -lGLU -lGL

const int screenWidth = 1024;
const int screenHeight = 800;

void handleInput(bool KEYS[], bool& gameState) {

	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_QUIT:
				gameState = false;
				break;
			case SDL_KEYDOWN:
				KEYS[event.key.keysym.sym] = true;
				break;
			case SDL_KEYUP:
				KEYS[event.key.keysym.sym] = false;
				break;
			default:
				break;
		}
	}

	if (KEYS[SDLK_ESCAPE]) {
		gameState = false;
		return;
	}
}

int main(int argc, char** argv) {

	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	auto* window = SDL_CreateWindow("Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, SDL_WINDOW_OPENGL);
	auto glContext = SDL_GL_CreateContext(window);

	GLenum err = glewInit();
	if(err != GLEW_OK) {
		std::cout << "Error: " << glewGetErrorString(err) << std::endl;
		std::cin.get();
		return -1;
	}

	// 322 is the number of SDLK_DOWN
	bool KEYS[322] = {};

	const int n = 4;
	
	Vertex vertices[n] = {
		Vertex{-0.5f, -0.5f, 0.0f,
		0.0f, 0.0f,
		1.0f, 0.0f, 0.0f, 1.0f},
		Vertex{-0.5f, 0.5f, 0.0f,
		0.0f, 1.0f,
		0.0, 1.0f, 0.0f, 1.0f},
		Vertex{0.5f, -0.5f, 0.0f,
		1.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 1.0f},
		Vertex{0.5f, 0.5f, 0.0f,
		1.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 1.0f},
	};

	VertexBuffer vertexBuffer(vertices, n);
	

	int indices[] = {
		0, 1, 2,
		1, 2, 3
	};
	int numIndices = 6;

	IndexBuffer indexBuffer(indices, numIndices, sizeof(indices[0]));

	int textureWidth = 0;
	int textureHeight = 0;
	int bitsPerPixel = 0;
	stbi_set_flip_vertically_on_load(true);
	auto textureBuffer = stbi_load("textures/box.png", &textureWidth, &textureHeight, &bitsPerPixel, 4);

	GLuint textureId;
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, textureWidth, textureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureBuffer);
	glBindTexture(GL_TEXTURE_2D, 0);

	if(textureBuffer) {
		stbi_image_free(textureBuffer);
	}

	std::cout << textureWidth << std::endl;
	std::cout << textureId << std::endl;

	Shader shader("shaders/basic.vs", "shaders/basic.fs");

	int colorUniformLocation = glGetUniformLocation(shader.getHandle(), "u_color");
	if(!colorUniformLocation != -1) {
		glUniform4f(colorUniformLocation, 1.0f, 0.0f, 1.0f, 1.0f);
	}

	int textureUniformLocation = glGetUniformLocation(shader.getHandle(), "u_texture");
	if(!textureUniformLocation != -1) {
		glUniform1i(textureUniformLocation, 0);
	}

	bool gameState = true;
	float time = 0.0f;

	while (gameState) {
		glClearColor(0,0.5,0.5,1);
		glClear(GL_COLOR_BUFFER_BIT);
		time += 0.1;

		if(!colorUniformLocation != -1) {
			glUniform4f(colorUniformLocation, sinf(time)*sinf(time), 0.0f, 1.0f, 1.0f);
		}

		vertexBuffer.bind();
		indexBuffer.bind();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureId);
		glDrawElements(GL_TRIANGLES, n, GL_UNSIGNED_INT, 0);
		indexBuffer.unbind();
		vertexBuffer.unbind();

		SDL_GL_SwapWindow(window);
		
		handleInput(KEYS, gameState);
	}

	glDeleteTextures(1, &textureId);

	return 0;
}
