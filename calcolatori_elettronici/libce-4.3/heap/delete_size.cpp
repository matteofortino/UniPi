#include "../internal.h"

void operator delete(void *p, size_t s)
{
	operator delete(p);
}
