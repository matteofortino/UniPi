#include "../internal.h"

int puts(const char *buf)
{
	vid::str_write(buf);
	vid::char_write('\n');
	vid::char_write('\r');
	return 1;
}
