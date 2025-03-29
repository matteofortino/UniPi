#include "../internal.h"

paddr trasforma(paddr root_tab, vaddr v)
{
	// usiamo un tab_iter sul solo indirizzo 'v', fermandoci
	// sul descrittore foglia lungo il percorso di traduzione
	tab_iter it(root_tab, v);
	while (it.down())
		;

	// si noti che il percorso potrebbe essere incompleto.
	// Ce ne accorgiamo perché il descrittore foglia ha il bit P a
	// zero. In quel caso restituiamo 0, che per noi non è un
	// indirizzo fisico valido.
	tab_entry e = it.get_e();
	if (!(e & BIT_P))
		return 0;

	// se il percorso è completo calcoliamo la traduzione corrispondente.
	// Si noti che non siamo necessariamente arrivati al livello 1, se
	// c'era un bit PS settato lungo il percorso.
	int l = it.get_l();
	natq mask = dim_region(l - 1) - 1;
	return (e & ~mask) | (v & mask);
}
