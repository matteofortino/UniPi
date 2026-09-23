#include "../internal.h"

void *operator new(size_t, std::align_val_t);
void *operator new[](size_t s, std::align_val_t a)
{
	return operator new(s, a);
}
