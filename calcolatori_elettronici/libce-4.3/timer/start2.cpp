#include "../internal.h"

namespace timer {

	void start2(natw N)
	{
		outputb(0b10110110, iCWR);        // contatore 2 in modo 3
		outputb(N, iCTR2_LOW);
		outputb(N >> 8, iCTR2_HIG);
	}

}
