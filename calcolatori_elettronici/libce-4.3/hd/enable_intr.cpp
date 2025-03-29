#include "../internal.h"

namespace hd {

	void enable_intr()
	{
		outputb(0x00, iDCR);
	}

}
