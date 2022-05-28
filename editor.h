#include <vector>
#include <time.h>
#include "buffer.h"

class CEditor {
private:
	int lastError = 0;
	int rows, cols;
	int cx, cy;
	int rx;
	int Read();
	int getCursorPosition();
	int getWindowSize();
	
	enum editorKey {
		BACKSPACE = 127,
		ARROW_LEFT = 1000,
		ARROW_RIGHT,
		ARROW_UP,
		ARROW_DOWN,
		DEL_KEY,
		HOME_KEY,
		END_KEY,
		PAGE_UP,
		PAGE_DOWN
	};
	
	struct BufferPool {
		Buffer *txtBuf;
		Buffer *renderBuf;
	};
	Buffer *abuf;
	int numrow;
	std::vector<BufferPool> bufPool;
	int rowoff;
	int coloff;
	char *filename;
	char statusmsg[80];
	time_t  statusmsg_time;
	char *saved_buffer;
	int dirty;
	
	void drawRows();
	void moveCursor(int);
	void addRow(char *s, size_t len);
	void scroll();
	void updateRow(BufferPool&);
	int mapToRender(BufferPool&, int);
	
	
	void drawStatusBar();
	void drawMessageBar();
	
	void insertChar(BufferPool&, int, int);
	void insertCharAtCursor(int c);	
	void delChar(BufferPool& pool, int at);
	void delCharAtCursor();
	void delRow(int);
	void insertRow(int, char *, size_t);
	void insertNewLine();
	void appendString(BufferPool&, char *, size_t);
	char *textToString(int *);
	
	char * prompt(char *);
	void save();

public:
	CEditor();
	~CEditor();

	void readfile(char *);

	void setStatusMessage(const char *fmt, ...);
	int onKeyPress();
	void refresh();
};
