#include <stdio.h>
#include <getopt.h>
#include "editor.h"
#include "rawmode.h"

static const char *def = NULL; //"editor.cpp";

int main(int argc, char *argv[]) {
	CEditor editor;
	char *fname = (char *)def;
	int c;
	
	while ((c = getopt(argc, argv, "d")) != -1) {
		switch (c) {
			case 'd':
				break;
			case '?':
				break;
			default:
				break;
		}
	}
	for (int i=optind; i<argc; i++) {
		fname = argv[i];
	}
	editor.readfile(fname);
	editor.setStatusMessage("HELP: Ctrl-S = save | Ctrl-Q = quit");
	while (1) {
		editor.refresh();
		if (editor.onKeyPress()) break;
	}
	return 0;
}
