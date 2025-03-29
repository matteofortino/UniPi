#include "../internal.h"

namespace svga {

	bool init()
	{
		natb bus = 0, dev = 0, fun = 0;
		natw deviceID = 0x1234;
		natw vendorID = 0x1111;

		//cerchiamo la scheda video basandoci sul deviceID e vendorID
		if(!pci::find_dev(bus, dev, fun, vendorID, deviceID))
		{
			//flog(LOG_WARN, "bochsvga: scheda video non rilevata");
			return false;
		}

		//indirizzo di memoria del framebuffer
		framebuffer = reinterpret_cast<volatile void*>(pci::read_confl(bus, dev, fun, 0x10) & ~0xf);
		//indirizzo dell'area di memoria in cui sono mappati i registri, 1:1, del controller VGA (non usati)
		vgareg = reinterpret_cast<volatile natw*>((pci::read_confl(bus, dev, fun, 0x18) & ~0xf) + 0x0400);
		//indirizzo dell'area di memoria in cui sono mappati i registri, 2:2, delle estensioni BOCHS VBE
		vbeext = reinterpret_cast<volatile natw*>((pci::read_confl(bus, dev, fun, 0x18) & ~0xf) + 0x0500);

		return true;
	}

}
