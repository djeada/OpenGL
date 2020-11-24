#include <iostream>
#define GLEW_STATIC
#include <GL/glew.h>
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

//compile with g++ -o main main.cpp -lSDL2main -lSDL2 -lGLEW -lGLU -lGL

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
	SDL_GLContext glContext = SDL_GL_CreateContext(window);

	 // 322 is the number of SDLK_DOWN
	bool KEYS[322] = {};

	bool gameState = true;
	while (gameState) {
		glClearColor(0,0.5,0.5,1);
		glClear(GL_COLOR_BUFFER_BIT);

		SDL_GL_SwapWindow(window);
		
		handleInput(KEYS, gameState);
	}

	return 0;
}
