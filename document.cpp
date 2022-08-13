#include <stdlib.h>
#include <stdio.h>
#include "document.h"

#define DEFAULT_BLOCK_SIZE 2048

Line::Line() : UString() {
	allocated = DEFAULT_BLOCK_SIZE;
	used = 0;
	data = (char *)calloc(sizeof(char), allocated);
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
Line::iterator Line::iterator::operator++() {
	if (cursor >= object->used) return *this;
	char *ptr = object->data + cursor;
	if (*ptr == '\0') return *this;
	cursor += object->byteCount(*ptr);
	return *this;
}

/**
 * Prefix increment one utf8 character
 */
Line::iterator Line::iterator::operator++(int) {
	if (cursor >= object->used) return *this;
	char *ptr = object->data + cursor;
	if (*ptr == '\0') return *this;
	Line::iterator it = *this;
	cursor += object->byteCount(*ptr);
	return it;
}

/**
 * Postfix decrement one utf8 character
 */
Line::iterator Line::iterator::operator--() {
	if (cursor < 1) return *this;
	int last = 0;
	int current = 0;
	char *ptr = object->data;
	while (current < cursor) {
		last = current;
		int nbytes = object->byteCount(*ptr);
		ptr += nbytes;
		current += nbytes;
	}
	return *this;
}

/**
 * Prefix decrement one utf8 character
 */
Line::iterator Line::iterator::operator--(int) {
	if (cursor < 1) return *this;
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
