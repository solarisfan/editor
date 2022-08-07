#ifndef SDLVIEW_H
#define SDLVIEW_H

#include <SDL.h>
#include "sdlfont.h"

class SDLview {
	private:
		SDL_Window *win;
		SDL_Renderer *renderer;
		int winWidth;
		int winHeight;
		int eventTimeout;
		SDL_Color bgcolor;
		SDL_Color fgcolor;
		
	protected:
		void waitNextEvent();
		SDLfont font;
		
	public:
		SDLview();
		~SDLview();
		void create();
		virtual void onKeyDown(SDL_Event& event) = 0;
};

#endif
