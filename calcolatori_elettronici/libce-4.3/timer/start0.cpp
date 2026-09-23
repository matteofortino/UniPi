#include "../internal.h"

namespace timer {

	void start0(natw N)
	{
		outputb(0b00110100, iCWR);        // contatore 0 in modo 2
		outputb(N, iCTR0_LOW);
		outputb(N >> 8, iCTR0_HIG);
	}

}
