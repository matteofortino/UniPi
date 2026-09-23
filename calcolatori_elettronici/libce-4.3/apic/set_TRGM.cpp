#include "../internal.h"

namespace apic {

	void set_TRGM(natl irq, bool v)
	{
		// lettura della riga irq-esima (parte bassa)
		// della tabella interna
		natl work = read_rtl(irq);
		// settiamo o resettiamo il bit TRGM in base
		// al valore di v
		if (v)
			work |= TRGM_BIT;
		else
			work &= ~TRGM_BIT;
		// riscrittura della riga irq-esima (parte bassa)
		// della tabella interna
		write_rtl(irq, work);
	}

}
