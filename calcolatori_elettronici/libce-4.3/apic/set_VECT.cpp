#include "../internal.h"

namespace apic {

	void set_VECT(natl irq, natb vec)
	{
		// lettura della prima parola lunga dell'entrata
		// irq-esima della tabella interna
		natl work = read_rtl(irq);
		// azzeramento (&) e predisposizione (|) dei bit 7-0 con vec
		work &= 0xFFFFFF00;
		work |= vec;
		// scrittura nella prima parola lunga dell'entrata
		// irq-esima della tabella interna
		write_rtl(irq, work);
	}

}
