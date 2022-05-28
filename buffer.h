#include <vector>

class Buffer {
	protected:
		char *b;
		int len;
		int allocated;
	public:
		Buffer() { b = NULL; len = 0; }
		~Buffer();
		void append(const char *s, int len);
		void append(const char *s);
		void write();
		int getLength() { return len; }
		char *getBytes() { return b; }
		void setBytes(char *s, int len);
		void insertChar(int at, int c);
		void delChar(int at);
		void truncate(int);
};
