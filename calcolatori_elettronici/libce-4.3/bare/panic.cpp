#include "../internal.h"

void panic(const char *msg)
{
	flog(LOG_ERR, "%s", msg);
	dump_status(LOG_ERR);
	end_program();
}
