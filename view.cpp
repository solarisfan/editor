
#include "view.h"

void TView::onKeyDown(SDL_Event& event) {
}

void TView::activate() {
	create();
	font.findDefaultFont();
	font.load();
	waitNextEvent();
}
