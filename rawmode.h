
#include <termios.h>

class CRawMode {
private:
	struct termios saved;

public:	
	~CRawMode();
	CRawMode();
	int enable();	
	void disable();
};

