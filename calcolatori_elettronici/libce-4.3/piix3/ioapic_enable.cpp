#include "../internal.h"

namespace piix3 {

	bool ioapic_enable(natb bus, natb dev, natb fun)
	{
		natl xbcs = pci::read_confl(bus, dev, fun, XBCS);
		xbcs |= XBCS_ENABLE;
		pci::write_confl(bus, dev, fun, XBCS, xbcs);
		return true;
	}

}
