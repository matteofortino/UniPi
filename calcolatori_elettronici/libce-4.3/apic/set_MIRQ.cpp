#include "../internal.h"

namespace apic {

	void set_MIRQ(natl irq, bool v)
	{
		// lettura della prima parola lunga dell'entrata
		// irq-esima della tabella interna
		natl work = read_rtl(irq);
		if (v)
			work |= MIRQ_BIT;
		else
			work &= ~MIRQ_BIT;
		// scrittura nella prima parola lunga dell'entrata
		// irq-esima della tabella interna
		write_rtl(irq, work);
	}

}
