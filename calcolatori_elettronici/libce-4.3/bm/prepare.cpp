#include "../internal.h"

namespace bm {

	void prepare(paddr prd, bool write)
	{
		outputl(prd, iBMDTPR);
		natb work = inputb(iBMCMD);
		if (write) {
			work &= ~0x8;
		} else {
			work |= 0x8;
		}
		outputb(work, iBMCMD);
		work = inputb(iBMSTR);
		work |= 0x6;
		outputb(work, iBMSTR);
	}

}
