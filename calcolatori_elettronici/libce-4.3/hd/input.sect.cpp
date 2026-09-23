#include "../internal.h"

namespace hd {

	void input_sect(natb vetto[])
	{
		wait_for_br();
		inputbw(iBR, reinterpret_cast<natw*>(vetto), 256);
	}

}
