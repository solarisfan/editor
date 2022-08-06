
#include "sdlview.h"

SDLview::SDLview() {
	winWidth = 640;
	winHeight = 480;
	eventTimeout = 1000; // 1 second
	SDL_Init(SDL_INIT_VIDEO);
}

SDLview::~SDLview() {
}

void SDLview::create() {
	Uint32 render_flags = SDL_RENDERER_ACCELERATED;
	win = SDL_CreateWindow("Untitled", SDL_WINDOWPOS_UNDEFINED,
				SDL_WINDOWPOS_UNDEFINED, winWidth, winHeight, 0);
	renderer = SDL_CreateRenderer(win, -1, render_flags);
}

void SDLview::waitNextEvent() {
	bool cont = true;
	SDL_Event event;
	
	SDL_StartTextInput();
	while (cont) {
		SDL_WaitEventTimeout(&event, eventTimeout);
		switch (event.type) {
			case SDL_QUIT:
				cont = false;
				break;
			case SDL_KEYDOWN: // capture special key
//				onKeyDown(event);
				break;
			case SDL_TEXTINPUT: // Actual text
//				onTextInput(event);
				break;
			case SDL_TEXTEDITING: // During character composition
				break;
		}
//		onIdle();
	}
	SDL_StopTextInput();
}
