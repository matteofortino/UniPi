#include "../internal.h"

namespace bm {

	void ack()
	{
		natb work = inputb(iBMCMD);
		work &= 0xFE;
		outputb(work, iBMCMD);
		inputb(iBMSTR);
	}

}
