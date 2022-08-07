#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <vector>
#include "ustring.h"

/**
 * A document contains paragraphs which contain lines of text.
 * A document contains a file handler.
 */
class Line {
	private:
		UString buf;
	public:
		Line() {};
		~Line() {};
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
		std::vector<Paragraph *>text;
		
	public:
		Document();
		~Document();
};

#endif
