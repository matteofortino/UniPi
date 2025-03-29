#include "../internal.h"

tab_iter::tab_iter(paddr tab, vaddr beg, natq dim)
{
	if (!valid_interval(beg, dim)) {
		panic("tab_iter: intervallo non valido");
		return;
	}

	vaddr end = beg + dim;

	// memorizzimo gli estremi dell'intervallo e inizializziamo
	// la condizione di fine visita
	pp()->cur = beg;
	pp()->end = end;
	pp()->tab = 0;

	if (dim == 0) {
		// se l'intervallo è vuoto ci portiamo direttamente in una
		// situazione di visita terminata
		l = MAX_LIV + 1;
		return;
	}

	l = MAX_LIV;
	sp()->tab = tab;
	sp()->cur = base(beg, l - 1);
	sp()->end = limit(end, l - 1);
}
