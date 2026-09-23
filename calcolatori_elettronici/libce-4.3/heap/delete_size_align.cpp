#include "../internal.h"

void operator delete(void *p, size_t, std::align_val_t)
{
	operator delete(p);
}
