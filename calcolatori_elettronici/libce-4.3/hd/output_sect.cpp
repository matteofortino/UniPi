#include "../internal.h"

namespace hd {

	void output_sect(natb vetto[])
	{
		wait_for_br();
		outputbw(reinterpret_cast<natw*>(vetto), 256, iBR);
	}

}
