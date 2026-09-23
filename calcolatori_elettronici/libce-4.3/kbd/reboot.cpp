#include "../internal.h"

namespace kbd {

	void reboot()
	{
		outputb(0xFE, iCMR);
		for (;;);
	}

}

extern "C" void reboot() {
	kbd::reboot();
	for (;;);
}
