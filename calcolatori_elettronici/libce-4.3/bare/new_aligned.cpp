#include "../internal.h"

void* operator new(size_t s, std::align_val_t a)
{
	return alloc_aligned(s, a);
}
