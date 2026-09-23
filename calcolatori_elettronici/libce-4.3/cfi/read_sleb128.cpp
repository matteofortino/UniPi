#include "../internal.h"

long read_sleb128(vaddr& s)
{
	long r = 0;
	natb b;
	unsigned shift = 0;
	do {
		b = *ptr_cast<natb>(s++);
		r |= (b & 0x7f) << shift;
		shift += 7;
	} while (b & 0x80);
	if (shift < sizeof(long) && (b & 0x40))
		r |= ~0UL << shift;
	return r;
}
