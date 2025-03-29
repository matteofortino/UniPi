#include "../internal.h"

namespace piix3 {

	bool ioapic_disable(natb bus, natb dev, natb fun)
	{
		natl xbcs = pci::read_confl(bus, dev, fun, piix3::XBCS);
		xbcs &= ~piix3::XBCS_ENABLE;
		pci::write_confl(bus, dev, fun, piix3::XBCS, xbcs);
		return true;
	}

}
