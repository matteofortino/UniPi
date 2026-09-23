#include "../internal.h"

void set_entry(paddr tab, natl j, tab_entry se)
{
	tab_entry& de = get_entry(tab, j);
	// il contatore deve essere aggiustato se il bit P cambia valore
	if ((se & BIT_P) && !(de & BIT_P)) {
		inc_ref(tab);
	} else if (!(se & BIT_P) && (de & BIT_P)) {
		dec_ref(tab);
	}
	de = se;
}
