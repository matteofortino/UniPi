#include "../internal.h"

namespace apic {

	void send_EOI() {
		// per inviare l'End Of Interrupt all'APIC è sufficiente
		// scrivere un valore qualsiasi nel registro EOIR.
		*pEOIR = 0;
	}

}

extern "C" void apic_send_EOI()
{
	apic::send_EOI();
}

