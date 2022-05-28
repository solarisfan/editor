
#include <unistd.h>
#include "rawmode.h"

int CRawMode::enable() {
	struct termios raw;
	int mask;
	int rc;
	
	raw = saved;
	mask = 0;
	mask |= ICRNL; // Stop input \r\n translation
	mask |= IXON; // Disable Ctrl-S Ctrl-Q
	raw.c_iflag &= ~(mask); 
	
	mask = 0;
	mask |= OPOST; // disable output \r\n translation
	raw.c_oflag &= ~(mask); 
	
	mask = 0;
	mask |= ECHO; // disable echo
	mask |= ICANON; // Single character input
	//mask |= ISIG; // Disable Ctrl-C Ctrl-Z
	//mask |= IEXTEN; // Disable Ctrl-V
	raw.c_lflag &= ~(mask);
	
	raw.c_cc[VMIN] = 0; // Minimum byte read
	raw.c_cc[VTIME] = 1; // Input wait time 1/10 of a second
	
	rc = tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
	return rc;
}

void CRawMode::disable() {
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &saved);
}

CRawMode::CRawMode() {
	tcgetattr(STDIN_FILENO, &saved);
}

CRawMode::~CRawMode() {
	disable();
}

