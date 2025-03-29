#include "../internal.h"

void* operator new(size_t s)
{
	return alloc(s);
}
