#include "../internal.h"

namespace pci {

	natb read_confb(natb bus, natb dev, natb fun, natb off)
	{
		natl confaddr = make_CAP(bus, dev, fun, off);
		outputl(confaddr, iCAP);
		return inputb(iCDP + (off & 0x03));
	}

}
