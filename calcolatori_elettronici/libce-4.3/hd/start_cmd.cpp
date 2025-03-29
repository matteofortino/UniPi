#include "../internal.h"

namespace hd {

	void start_cmd(natl lba, natb n, cmd c)
	{
		set_lba(lba);
		outputb(n, iSCR);
		outputb(c, iCMD);
	}

}
