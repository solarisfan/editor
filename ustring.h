#ifndef USTRING_H
#define USTRING_H

/**
 * Class for handling utf-8 encoded string.
 * All memory allocation is done by calling program.
 */
class UString {
	protected:
		char *data;
		int byteCount(char c);
		
	public:
		UString();
		UString(char *s);
		~UString();
		
		UString& operator = (const char *s);
		operator char*() const { return data; };
		
		char *getBytesBuffer() { return data; };
		void setByteBuffer(char *s) { data = s; };
		
		int strlen();  // Can return -1 on invalid string
};

#endif
