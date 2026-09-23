#include "../internal.h"

void copy_des(paddr src, paddr dst, natl i, natl n)
{
	for (natl j = i; j < i + n && j < 512; j++) {
		tab_entry se = get_entry(src, j);
		set_entry(dst, j, se);
	}
}
