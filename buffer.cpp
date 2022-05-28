#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "buffer.h"
#include <stdio.h>

Buffer::~Buffer() {
	if (b) free(b); 
	b = NULL;
	len = 0;
	allocated = 0;
}

void Buffer::append(const char *s, int nbytes) {
	if (nbytes <= 0) return;
	if (!s) return;
	if (allocated < 1) allocated = 1;
	allocated += nbytes;
	char *tmp = (char *)realloc(b, allocated);
	if (tmp == NULL) return;
	memcpy(&tmp[len], s, nbytes);
	b = tmp;
	len += nbytes;
	b[len] = '\0';
}

void Buffer::append(const char *s) {
	if (!s) return;
	if (!(*s)) return;
	append(s, strlen(s));
}

void Buffer::write() {
	if ((b) && (len > 0)) {
		::write(STDOUT_FILENO, b, len);
	}
}

void Buffer::setBytes(char *s, int nbytes) {
	if (b) free(b); 
	b = NULL;
	len = 0;
	allocated = 0;
	append(s, nbytes);
}

void Buffer::insertChar(int at, int c) {
	int idx = at;
	if ((at < 0) || (at > len)) idx = len;
	allocated = len + 2;
	b = (char *)realloc(b, allocated);
	memmove(b+idx+1, b+idx, len - idx + 1);
	len++;
	b[idx] = c;
}

void Buffer::delChar(int at) {
	int idx = at;
	if ((at < 0) || (at >= len)) return;
	memmove(b+idx, b+idx+1, len - idx);
	len--;
}

void Buffer::truncate(int nbytes) {
	*(b+nbytes) = '\0';
	len = nbytes;
}
