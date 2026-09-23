#include "../internal.h"

namespace apic {

	bool init()
	{
		natb bus = 0, dev = 0, fun = 0;
		if (!pci::find_dev(bus, dev, fun, piix3::DEVICE_ID, piix3::VENDOR_ID)) {
			flog(LOG_ERR, "PIIX3 non trovato");
			return false;
		}
		if (!piix3::ioapic_enable(bus, dev, fun)) {
			flog(LOG_ERR, "Impossibile abilitare l'APIC nel PIIX3");
			return false;
		}
		for (natb i = 0; i < IRQ_MAX; i++) {
			write_rth(i, 0);
			write_rtl(i, MIRQ_BIT);
		}
		return true;
	}

}
