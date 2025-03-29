#include "../internal.h"

namespace pci {

	natw read_confw(natb bus, natb dev, natb fun, natb off)
	{
		natl confaddr = make_CAP(bus, dev, fun, off);
		outputl(confaddr, iCAP);
		return inputw(iCDP + (off & 0x03));
	}

}
