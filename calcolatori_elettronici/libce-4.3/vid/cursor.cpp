#include "../internal.h"

namespace vid {

	void cursor()
	{
		natw pos = COLS * y + x;
		outputb(CUR_HIGH, iIND);
		outputb((natb)(pos >> 8), iDAT);
		outputb(CUR_LOW, iIND);
		outputb((natb)pos, iDAT);
	}

}
