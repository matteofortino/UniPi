#include "../internal.h"

namespace timer {

	void disable_spk()
	{
		outputb(0, iSPR);
	}

}
