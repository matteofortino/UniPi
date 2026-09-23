#include "../internal.h"

namespace pci {

	bool find_dev(natb& bus, natb& dev, natb& fun, natw devID, natw vendID)
	{
		do {
			natw work;

			if ( (work = read_confw(bus, dev, fun, 0)) == 0xFFFF )
				continue;
			if ( work == vendID && read_confw(bus, dev, fun, 2) == devID)
				return true;
		} while (next(bus, dev, fun));
		return false;
	}

}
