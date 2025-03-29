#include "../internal.h"

void tab_iter::post()
{
	if (done())
		return;

	// scendiamo seguendo il primo ramo a sinistra, fino alla foglia
	while (down())
		;
}
