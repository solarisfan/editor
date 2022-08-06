#ifndef VIEW_H
#define VIEW_H

#include "sdlview.h"

class TView : public SDLview {
	public:
		virtual void onKeyDown(SDL_Event& event);
		void activate();
};

#endif
