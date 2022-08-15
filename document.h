#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <vector>
#include "ustring.h"

/**
 * A line of text contains arbiturary number of utf8 characters.
 * New line character is not encoded.
 * A paragraph contains a number of lines
 * A document contains a file handler.
 */
class Line : public UString {
	private:
		int allocated;
		int used;
		int cursor; // # of bytes from pointer start
		char *index;
		int addOneChar(char *s);
		
	protected:
		void checkBuffer(size_t bytesRequired);
		
	public:
		class iterator {
			private:
				int cursor;
				Line *object;
				
			public:
				iterator(Line *obj);
				Line::iterator& operator++(); //Prefix 
				Line::iterator operator++(int); //Postfix
				Line::iterator& operator--(); //Prefix 
				Line::iterator operator--(int); //Postfix
				bool eol();
		};
		
		Line::iterator begin(int pos = 0); // # of utf8 characters offset
		Line();
		~Line();
		void setPos(int bytes); // Move cursor to the byte position
		Line& operator +=(char *s);
};

class Paragraph {
	private:
		std::vector<Line *>line;
		
	public:
		Paragraph() {};
		~Paragraph() {};
};

class Document {
	private:
		Paragraph text;
		
	public:
		Document();
		~Document();
		void load(char *s);
};

#endif
