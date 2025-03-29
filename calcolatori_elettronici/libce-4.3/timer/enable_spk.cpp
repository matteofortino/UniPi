#include "../internal.h"

namespace timer {

	void enable_spk()
	{
		outputb(3, iSPR);
	}

}
