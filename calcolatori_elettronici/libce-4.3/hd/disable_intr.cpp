#include "../internal.h"

namespace hd {

	void disable_intr()
	{
		outputb(0x02, iDCR);
	}

}
