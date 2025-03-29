#include "../internal.h"

// Per allocare una taballe di intessa solo la dimensione e l'allineamento.
struct alignas(DIM_PAGINA) TAB {
	char tab[DIM_PAGINA];
};

// versione di default di alloca_tab(): usa lo heap
paddr alloca_tab()
{
	void* tab = new TAB;
	if (!tab)
		return 0;

	memset(tab, 0, DIM_PAGINA);

	return int_cast<paddr>(tab);
}
