#include "../internal.h"

namespace pci {

	void write_confl(natb bus, natb dev, natb fun, natb off, natl data)
	{
		natl confaddr = make_CAP(bus, dev, fun, off);
		outputl(confaddr, iCAP);
		outputl(data, iCDP);
	}

}
