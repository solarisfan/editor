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
	char *idx = object->bytemap + cursor;
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
	char *idx = object->bytemap + cursor;
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

Line& Line::operator +=(char *s) {
	size_t len = ::strlen(s);
	checkBuffer(len + used);
	if (cursor < used) {
		char *src = data + cursor;
		::memmove(src + len, src, ::strlen(src));
		src = bytemap + cursor;
		::memmove(src + len, src, ::strlen(src));
	}
	char *ptr = data + cursor;
	char *bits = bytemap + cursor;
	while(len > 0) {
		int n = byteCount(*s);
		len -= n;
		*bits = n;
		*ptr = *s;
		cursor++;
		n--;
		s++;
		while (n > 0) {
			ptr = data + cursor;
			bits = bytemap + cursor;
			*ptr = *s;
			*bits = 0;
			s++;
			n--;
			cursor++;
		}
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
