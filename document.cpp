#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "document.h"

#define DEFAULT_BLOCK_SIZE 2048

Line::Line() : UString() {
	allocated = DEFAULT_BLOCK_SIZE;
	used = 0;
	data = (char *)calloc(sizeof(char), allocated);
	bytemap = (char *)calloc(sizeof(char), allocated);
	cursor = 0;
}

Line::~Line() {
	if (data) free(data);
	data = NULL;
	cursor = 0;
}

void 
Line::checkBuffer(size_t bytesRequired) {
	size_t newsize = allocated;
	while (bytesRequired >= newsize) newsize += DEFAULT_BLOCK_SIZE;
	if (bytesRequired > (size_t)allocated) {
		allocated = newsize;
		char *tmp = (char *)realloc(data, newsize);
		data = tmp;
		tmp = (char *)realloc(bytemap, newsize);
		bytemap = tmp;
	}
}

void
Line::setPos(int bytes) {
	if (!data) return;
	cursor = bytes;
}

Line::iterator::iterator(Line *o) {
	object = o;
	cursor = 0;
}

Line::iterator Line::begin(int pos) {
	Line::iterator it(this);
	return it;
}

bool Line::iterator::eol() {
	if (cursor >= object->used) return true;
	return false;
}

/**
 * Postfix increment one utf8 character
 */
Line::iterator Line::iterator::operator++(int) {
	if (cursor >= object->used) return *this;
	Line::iterator it = *this;
	char *ptr = object->data + cursor;
	char *idx = object->index + cursor;
	if (*ptr == '\0') return it;
	cursor += *idx;
	return it;
}

/**
 * Prefix increment one utf8 character
 */
Line::iterator& Line::iterator::operator++() {
	if (cursor >= object->used) return *this;
	char *ptr = object->data + cursor;
	char *idx = object->index + cursor;
	if (*ptr == '\0') return *this;
	cursor += *idx;
	return *this;
}

/**
 * Postfix decrement one utf8 character
 */
Line::iterator Line::iterator::operator--(int) {
	if (cursor < 1) return *this;
	Line::iterator it = *this;
	int last = 0;
	int current = 0;
	char *ptr = object->data;
	while (current < cursor) {
		last = current;
		int nbytes = object->byteCount(*ptr);
		ptr += nbytes;
		current += nbytes;
	}
	return it;
}

/**
 * Prefix decrement one utf8 character
 */
Line::iterator& Line::iterator::operator--() {
	if (cursor < 1) return *this;
	return *this;
}

/**
 * Add one utf8 character to the current cursor position
 */
int Line::addOneChar(char *s) {
	char *ptr = data + cursor;
	char *bits = index + cursor;
	char tmp[5];
	
	int n = byteCount(*s);
	if (n < 1) return 0;
	memcpy(tmp, s, n);
	*(tmp + n) = '\0';
	memcpy(ptr, tmp, n);
	memset(bits, 0, n);
	*bits = n;
	cursor += n;
	used += n;
	*(ptr + used) = '\0';
	return n;
}

/**
 * Add character(s) to the current cursor.
 * Input method can have word association.
 */
Line& Line::operator +=(char *s) {
	size_t len = ::strlen(s);
	checkBuffer(len + used);
	if (cursor < used) {
		char *src = data + cursor;
		::memmove(src + len, src, ::strlen(src));
		src = index + cursor;
		::memmove(src + len, src, ::strlen(src));
	}
	while(len > 0) {
		int n = addOneChar(s);
		len -= n;
		if (len > 0) s += n;
	}
	return *this;
}

Document::Document() {
}

Document::~Document() {
}

void Document::load(char *s) {
	if ((!s) || (!*s)) return;
	FILE *fp = fopen(s, "r");
	if (!fp) {
		printf("Cannot open file %s\n", s);
		return;
	}
	while (!feof(fp)) {
		int c = fgetc(fp);
	}
	fclose(fp);
}
