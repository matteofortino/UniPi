#include "../internal.h"

namespace bm {

	void start()
	{
		natb work = inputb(iBMCMD);
		work |= 1;
		outputb(work, iBMCMD);
	}

}
