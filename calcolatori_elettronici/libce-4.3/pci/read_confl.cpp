#include "../internal.h"

namespace pci {

	natl read_confl(natb bus, natb dev, natb fun, natb off)
	{
		natl confaddr = make_CAP(bus, dev, fun, off);
		outputl(confaddr, iCAP);
		return inputl(iCDP);
	}

}
