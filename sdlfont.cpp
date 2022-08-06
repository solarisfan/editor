
#include <string.h>
#include <stdio.h>
#include "sdlfont.h"
#include <fontconfig/fontconfig.h>

SDLfont::SDLfont() {
	fontPath = NULL;
//	fontFamily = strdup("Courier");
	fontFamily = strdup("Px IBM VGA9");
	fontSize = 32;
	font = NULL;
	if (TTF_Init() == -1) {
		printf("TTF init failed\n");
	}
}

SDLfont::~SDLfont() {
	if (font) TTF_CloseFont(font);
	font = NULL;
}

void SDLfont::findDefaultFont() {
	FcInit();
	FcConfig *conf = FcConfigGetCurrent();
	
	// Load a default font first
	FcPattern *pat = FcNameParse((const FcChar8*)fontFamily);
	if (pat == NULL) {
		printf("Error creating pattern\n");
	}
	FcConfigSubstitute(0, pat, FcMatchPattern);
	FcDefaultSubstitute(pat);
	FcResult rc;
	FcPattern *matched = FcFontMatch(0, pat, &rc);
	if (matched) {
		FcChar8 *path = NULL;
		FcPatternGetString(matched, FC_FILE, 0, &path);
		printf("%s\n", path);
		fontPath = strdup((char *)path);
	}
	FcPatternDestroy(matched);
	FcPatternDestroy(pat);
	FcConfigDestroy(conf);
	FcFini();
}

int SDLfont::load() {
	font = TTF_OpenFont(fontPath, fontSize);
	if (!font) {
		char *s = (char *)TTF_GetError();
		printf("Open font error %s\n", s);
		return 1;
	}
	return 0;
}

