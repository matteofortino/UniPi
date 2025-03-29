#include "../internal.h"

void operator delete(void *p)
{
	dealloc(p);
}
