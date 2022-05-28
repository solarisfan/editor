#include <unistd.h>
#include <sys/ioctl.h>
#include "editor.h"
#include <errno.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <fcntl.h>
#include "rawmode.h"

#define CTRL_KEY(k) ((k) & 0x1f)
#define TAB_STOP 8
#define QUIT_TIMES 2

static CRawMode rawMode;

void CEditor::readfile(char *s) {
	if (s == NULL) return;
	if (filename) free(filename);
	filename = strdup(s);
	FILE *fp = fopen(s, "r");
	if (!fp) return;
	char *line = NULL;
	size_t linecap = 0;
	ssize_t linelen;
	while ((linelen = getline(&line, &linecap, fp)) != -1) {
		while ((linelen > 0) && ((line[linelen - 1] == '\n') ||
								(line[linelen - 1] == '\r'))) {
			linelen--;
		}
		addRow(line, linelen);
	}
	free(line);
	fclose(fp);
	dirty = 0;
}

CEditor::CEditor() {
	abuf = NULL;
	cx = cy = 0;
	numrow = 0;
	rowoff = 0;
	coloff = 0;
	rx = 0;
	filename = NULL;
	rawMode.enable();
	getWindowSize();
	rows -= 2;
	statusmsg[0] = '\0';
	statusmsg_time = 0;
	saved_buffer = NULL;
	dirty = 0;
}

CEditor::~CEditor() {
	if (filename) free(filename);
	filename = NULL;
	if (abuf) delete abuf;
	if (saved_buffer) free(saved_buffer);
	saved_buffer = NULL;
	while (!bufPool.empty()) {
		BufferPool p = bufPool.back();
		if (p.txtBuf) delete p.txtBuf;		
		if (p.renderBuf) delete p.renderBuf;
		p.txtBuf = NULL;
		p.renderBuf = NULL;
		bufPool.pop_back();
	}
}

int CEditor::Read() {
	int nread;
	char c;
	
	lastError = 0;
	while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
		if ((nread == -1) && (errno != EAGAIN)) lastError = errno;
	}
	if (c == '\x1b') {
		char seq[3];
		
		if (read(STDIN_FILENO, &seq[0], 1) != 1) return '\x1b';
		if (read(STDIN_FILENO, &seq[1], 1) != 1) return '\x1b';
		
		if (seq[0] == '[') {
			if ((seq[1] >= '0') && (seq[1] <= '9')) {
				if (read(STDIN_FILENO, &seq[2], 1) != 1) return '\x1b';
				if (seq[2] == '~') {
					switch (seq[1]) {
						case '1': return HOME_KEY;
						case '3': return DEL_KEY;
						case '4': return END_KEY;
						case '5': return PAGE_UP;
						case '6': return PAGE_DOWN;
						case '7': return HOME_KEY;
						case '8': return END_KEY;
					}
				}
			} else {
				switch (seq[1]) {
					case 'A': return ARROW_UP;
					case 'B': return ARROW_DOWN;
					case 'C': return ARROW_RIGHT;
					case 'D': return ARROW_LEFT;
					case 'H': return HOME_KEY;
					case 'F': return END_KEY;
				
				}
			}
		} else if (seq[0] == 'O') {
			switch (seq[1]) {
				case 'H': return HOME_KEY;
				case 'F': return END_KEY;
			}
		}
		return '\x1b';
	}
	return c;
}

int CEditor::onKeyPress() {
	static int quit_times = QUIT_TIMES;
	int c = Read();
	
	if (lastError != 0) return 1;
	switch(c) {
		case '\r':
			insertNewLine();
			break;
		case CTRL_KEY('q'):
			if (dirty && (quit_times > 0)) {
				setStatusMessage("There are unsaved changes. "
								"Press Ctrl-Q %d more times to quit.",
								quit_times);
				quit_times--;
				return 0;
			}
			write(STDOUT_FILENO, "\x1b[2J", 4);
			write(STDOUT_FILENO, "\x1b[H", 3);
			return 1;
			break;
		case CTRL_KEY('s'):
			save();
			break;
		case HOME_KEY:
			cx = 0;
			break;
		case END_KEY:
			if (cy < numrow) {
				cx = bufPool[cy].txtBuf->getLength();
			}
			break;
		case BACKSPACE:
		case CTRL_KEY('h'):
		case DEL_KEY:
			if (c == DEL_KEY) moveCursor(ARROW_RIGHT);
			delCharAtCursor();
			break;
		case PAGE_UP:
		case PAGE_DOWN:
			{ 
				if (c == PAGE_UP) {
					cy = rowoff;
				} else if (c == PAGE_DOWN) {
					cy = rowoff + rows - 1;
					if (cy > numrow) cy = numrow;
				}
				int times = rows;
				while (times--)
					moveCursor(c == PAGE_UP ? ARROW_UP : ARROW_DOWN);
			}
			break;
		case ARROW_UP:
		case ARROW_DOWN:
		case ARROW_LEFT:
		case ARROW_RIGHT:
			moveCursor(c);
			break;
		default:
			insertCharAtCursor(c);
			break;
	}
	quit_times = QUIT_TIMES;
	return 0;
}

void CEditor::drawRows() {
	int y;
	for(y = 0; y < rows; y++) {
		int filerow = y + rowoff;
		if (filerow >= numrow) {
			if ((numrow == 0) && (y == (rows / 3))) {
				char welcome[80];
				int n = snprintf(welcome, sizeof(welcome), "Editor version 1.0");
				if (n > cols) n = cols;
				int padding = (cols - n) / 2;
				if (padding) {
					abuf->append("~", 1);
					padding --;
				}
				while (padding--) abuf->append(" ", 1);
				abuf->append(welcome, n);
			} else {
				abuf->append("~", 1);
			}
		} else {
			Buffer *tmp = bufPool[filerow].renderBuf;
			int n = tmp->getLength() - coloff;
			if (n < 0) n = 0;
			if (n > cols) n = cols;
			char *s = tmp->getBytes();
			abuf->append(s+coloff, n);
		}
		
		abuf->append("\x1b[K", 3);
		abuf->append("\r\n", 2);
	}
}

void CEditor::refresh() {
	scroll();
	abuf = new Buffer();
	abuf->append("\x1b[?25l", 6);
	abuf->append("\x1b[H", 3);
	drawRows();
	drawStatusBar();
	drawMessageBar();
	char buf[32];
	snprintf(buf, sizeof(buf), "\x1b[%d;%dH", (cy - rowoff) + 1, 
					(rx - coloff) + 1);
	abuf->append(buf, strlen(buf));
	abuf->append("\x1b[?25h", 6);
	abuf->write();
	delete abuf;
	abuf = NULL;
}

int CEditor::getWindowSize() {
	struct winsize ws;
	
	if ((ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1) || (ws.ws_col == 0)) {
		if (write(STDOUT_FILENO, "\x1b[999C\x1b[999B", 12) != 12) return -1;
		return getCursorPosition();
	}
	cols = ws.ws_col;
	rows = ws.ws_row;
	return 0;
}

int CEditor::getCursorPosition() {
	char buf[32];
	unsigned int i = 0;
	
	if (write(STDOUT_FILENO, "\x1b[6n", 4) != 4) return -1;
	while (i < sizeof(buf) -1) {
		if (read(STDIN_FILENO, &buf[i], 1) != 1) break;
		if (buf[i] == 'R') break;
		i++;
	}
	buf[i] = '\0';
	if ((buf[0] != '\x1b') || (buf[1] != '[')) return -1;
	if (sscanf(&buf[2], "%d;%d", &rows, &cols) != 2) return -1;
	return -1;
}


void CEditor::moveCursor(int key) {
	Buffer *ln = NULL;
	if (cy < numrow) ln = bufPool[cy].txtBuf;
	switch(key) {
		case ARROW_LEFT:
			if (cx != 0) {
				cx--;
			} else if (cy > 0) {
				cy--;
				cx = ln->getLength();
			}
			break;
		case ARROW_RIGHT:
			if (ln && (cx < ln->getLength())) {
				cx++;
			} else if (ln && (cx == ln->getLength())) {
				cy++;
				cx = 0;
			}
			break;
		case ARROW_UP:
			if (cy != 0) {
				cy--;
			}
			break;
		case ARROW_DOWN:
			if (cy < numrow) {
				cy++;
			}
			break;
	}
	int rowlen = 0;
	if (ln) rowlen = ln->getLength();
	if (cx > rowlen) cx = rowlen;
}

void CEditor::addRow(char *s, size_t len) {
	BufferPool pool;
	Buffer *tmp = new Buffer;
	tmp->append(s, len);
	pool.txtBuf = tmp;
	tmp = new Buffer;
	pool.renderBuf = tmp;
	updateRow(pool);
	bufPool.push_back(pool);
	numrow++;
	dirty++;
}

void CEditor::scroll() {
	rx = 0;
	if (cy < numrow) {
		rx = mapToRender(bufPool[cy], cx);
	}
	if (cy < rowoff) {
		rowoff = cy;
	}
	if (cy > (rowoff + rows)) {
		rowoff = cy - rows + 1;
	}
	if (rx < coloff) {
		coloff = rx;
	}
	if (rx >= coloff + cols) {
		coloff = rx - cols + 1;
	}
}

void CEditor::updateRow(BufferPool& obj) {
	int tabs = 0;
	int len = obj.txtBuf->getLength();
	char *txt = obj.txtBuf->getBytes();
	
	for(int j=0; j<len; j++) {
		if (txt[j] == '\t') tabs++;
	}
	char *s = (char *)malloc(len + tabs*(TAB_STOP - 1) + 1);	
	int idx = 0;
	for (int j=0; j < len; j++) {
		if (txt[j] == '\t') {
			s[idx++] = ' ';
			while ((idx % TAB_STOP) != 0) s[idx++] = ' ';
		} else {
			s[idx++] = txt[j];
		}
	}
	s[idx] = '0';
	obj.renderBuf->setBytes(s, idx);
	free(s);
}

int CEditor::mapToRender(BufferPool& pool, int x) {
	int x2 = 0;
	
	char *s = pool.txtBuf->getBytes();
	for (int j=0; j<x; j++) {
		if (s[j] == '\t') {
			x2 += (TAB_STOP - 1) - (x2 % TAB_STOP);
		}
		x2++;
	}
	return x2;
}

void CEditor::drawStatusBar() {
	char status[80], rstatus[80];
	int len, rlen;
	
	if (!abuf) return;
	abuf->append("\x1b[7m", 4);
	
	len = snprintf(status, sizeof(status), "%.20s - %d lines %s",
				filename ? filename : "[Untitles]", numrow,
				dirty ? "(modified)" : "");
	rlen = snprintf(rstatus, sizeof(rstatus), "%d/%d", cy+1, numrow);			
	if (len > cols) len = cols;
	abuf->append(status, len);	
	while (len < cols) {
		if ((cols - len) == rlen) {
			abuf->append(rstatus, rlen);
			break;
		} else {
			abuf->append(" ", 1);
			len++;
		}
	}
	abuf->append("\x1b[m", 3);
	abuf->append("\r\n", 2);
}

void CEditor::setStatusMessage(const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(statusmsg, sizeof(statusmsg), fmt, ap);
	va_end(ap);
	statusmsg_time = time(NULL);
}

void CEditor::drawMessageBar() {
	abuf->append("\x1b[K", 3);
	int msglen = strlen(statusmsg);
	if (msglen > cols) msglen = cols;
	if (msglen && ((time(NULL) - statusmsg_time) < 5)) {
		abuf->append(statusmsg, msglen);
	}
}

void CEditor::insertChar(BufferPool& pool, int at, int c) {
	Buffer *txt = pool.txtBuf;
	txt->insertChar(at, c);
	updateRow(pool);
	dirty++;
}

void CEditor::insertCharAtCursor(int c) {
	if (cy == numrow) {
		insertRow(numrow, NULL, 0);
	}
	insertChar(bufPool[cy], cx, c);
	cx++;
}

void CEditor::delChar(BufferPool& pool, int at) {
	Buffer *txt = pool.txtBuf;
	txt->delChar(at);
	updateRow(pool);
	dirty++;
}

void CEditor::delCharAtCursor() {
	if (cy == numrow) return;
	if ((cx == 0) && (cy == 0)) return;
	if (cx > 0) {
		delChar(bufPool[cy], cx-1);
		cx--;
	} else {
		cx = bufPool[cy-1].txtBuf->getLength();
		appendString(bufPool[cy-1], bufPool[cy].txtBuf->getBytes(),
				bufPool[cy].txtBuf->getLength());
		delRow(cy);
		cy--;
	}
}

char * CEditor::textToString(int *buflen) {
	int totlen = 0;
	int j;
	
	for(j=0; j<numrow; j++) {
		totlen += bufPool[j].txtBuf->getLength() + 1;
	}
	*buflen = totlen;
	
	char *buf = (char *)malloc(totlen);
	char *p = buf;
	for(j=0; j<numrow; j++) {
		int n = bufPool[j].txtBuf->getLength();
		memcpy(p, bufPool[j].txtBuf->getBytes(), n);
		p += n;
		*p = '\n';
		p++;
	}
	saved_buffer = buf;
	return buf;
}

void CEditor::save() {
	if (!filename) {
		filename = prompt((char *)"Save as: %s (ESC to cancel)");
		if (filename == NULL) {
			setStatusMessage((char *)"Save aborted");
			return ;
		}
	}
	if (!filename) return;
	int len;
	char *buf = textToString(&len);
	
	int fd = open(filename, O_RDWR | O_CREAT, 0644);
	if (fd == -1) return;
	if (ftruncate(fd, len) == -1) {
		close(fd);
		return;
	}
	if (write(fd, buf, len) == len) {
		setStatusMessage("%d bytes written to disk", len);
		dirty = 0;
	} else {
		setStatusMessage("File not saved. %s", strerror(errno));
	}
	close(fd);
	free(buf);
	saved_buffer = NULL;
}

void CEditor::delRow(int at) {
	if ((at < 0) || (at >= numrow)) return;
	BufferPool pool = bufPool[at];
	delete pool.txtBuf;
	delete pool.renderBuf;
	bufPool.erase(bufPool.begin()+at);
	numrow--;
	dirty++;
}

void CEditor::appendString(BufferPool& pool, char *s, size_t len) {
	pool.txtBuf->append(s, len);
	updateRow(pool);
	dirty++;
}

void CEditor::insertRow(int at, char *s, size_t len) {
	if ((at < 0) || (at > numrow)) return;
	BufferPool pool;
	Buffer *tmp = new Buffer;
	tmp->append(s, len);
	pool.txtBuf = tmp;
	tmp = new Buffer;
	pool.renderBuf = tmp;
	updateRow(pool);
	if (bufPool.empty()) {
		bufPool.push_back(pool);
	} else {
		bufPool.insert(bufPool.begin() + at, pool);
	}
	numrow++;
	dirty++;
}

void CEditor::insertNewLine() {
	if (cx == 0) {
		insertRow(cy, NULL, 0);
	} else {
		BufferPool pool = bufPool[cy];
		char *s = pool.txtBuf->getBytes();
		int len = pool.txtBuf->getLength();
		insertRow(cy+1, s+cx, len - cx);
		pool.txtBuf->truncate(cx);
		updateRow(pool);
	}
	cy++;
	cx = 0;
}

char * CEditor::prompt(char *p) {
	size_t bufsize = 128;
	char *buf = (char *)malloc(bufsize);
	size_t buflen = 0;
	buf[0] = '\0';
	
	while (1) {
		setStatusMessage(p, buf);
		refresh();
		
		int c = Read();
		if ((c == DEL_KEY) || (c == CTRL_KEY('h')) || (c == BACKSPACE)) {
			if (buflen != 0) buf[--buflen] = '\0';
		} else if (c == '\x1b') {
			setStatusMessage(NULL);
			free(buf);
			return NULL;
		} else if (c == '\r') {
			if (buflen != 0) {
				setStatusMessage("");
				return buf;
			}
		} else if (!iscntrl(c) && (c < 128)) {
			if (buflen == (bufsize - 1)) {
				bufsize *= 2;
				buf = (char *)realloc(buf, bufsize);
			}
			buf[buflen++] = c;
			buf[buflen] = '\0';
		}
	}
}
