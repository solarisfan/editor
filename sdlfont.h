#ifndef SDLFONT_H
#define SDLFONT_H

#include <SDL_ttf.h>

class SDLfont {
	private:
		char *fontPath;
		char *fontFamily;
		int fontSize;
		TTF_Font *font;

	public:
		SDLfont();
		~SDLfont();
		void findDefaultFont();
		int load();
};

#endif
