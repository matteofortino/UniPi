#include "../internal.h"

void operator delete[](void *p)
{
	operator delete(p);
}
