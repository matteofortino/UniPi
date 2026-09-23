#include "../internal.h"

namespace bm {

	bool find(natb& bus, natb& dev, natb& fun)
	{
		natb code[] = { 0xff, 0x01, 0x01 };

		do {
			if (pci::find_class(bus, dev, fun, code) && (code[0] & (1U << 7)))
				return true;
		} while (pci::next(bus, dev, fun));
		return false;
	}

}
