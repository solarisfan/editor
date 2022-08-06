#include <stdio.h>
#include <getopt.h>
#include "view.h"

int main(int argc, char *argv[]) {
	TView vw;
	char *fname = NULL;
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
	vw.activate();
	return 0;
}
