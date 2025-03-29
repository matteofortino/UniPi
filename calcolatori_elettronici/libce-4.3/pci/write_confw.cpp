#include "../internal.h"

namespace pci {

	void write_confw(natb bus, natb dev, natb fun, natb off, natw data)
	{
		natl confaddr = make_CAP(bus, dev, fun, off);
		outputl(confaddr, iCAP);
		outputw(data, iCDP + (off & 0x03));
	}

}
