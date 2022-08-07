
#include <string.h>
#include "ustring.h"

static int byteCount(char s) {
	int nbytes = 0;
	if (s < 0x80) {
		nbytes = 1;
	} else if ((s & 0xF0) == 0xF0) {
		nbytes = 4;
	} else if ((s & 0xE0) == 0xE0) {
		nbytes = 3;
	} else if ((s & 0xC0) == 0xC0) {
		nbytes = 2;
	}
	return nbytes;
}

UString::UString() {
	data = 0;
}

UString::UString(char *s) {
	data = s;
}

UString::~UString() {
	data = 0;
}

UString& UString::operator= (const char *s) {
	data = (char *)s;
	return *this;
}

int UString::strlen() {
	if (!data) return 0;
	int limit = ::strlen(data);
	char *s = data;
	int cnt=0;
	for (int i=0; i < limit; cnt++) {
		int bytes = ::byteCount(*s);
		if (bytes == 0) return -1;
		i += bytes;
		s += bytes;
	}
	return cnt;
	
}
