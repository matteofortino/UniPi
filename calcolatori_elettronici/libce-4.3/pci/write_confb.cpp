#include "../internal.h"

namespace pci {

	void write_confb(natb bus, natb dev, natb fun, natb off, natb data)
	{
		natl confaddr = make_CAP(bus, dev, fun, off);
		outputl(confaddr, iCAP);
		outputb(data, iCDP + (off & 0x03));
	}

}
