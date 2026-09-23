#include "../internal.h"

bool tab_iter::right()
{
	if (done() || sp()->cur == sp()->end)
		return false;

	// per andare a destra ci limitiamo a spostare cur della dimensione
	// delle pagine gestite dalle entrate del livello corrente
	sp()->cur += dim_region(l - 1);

	// se cur è diventato uguale a end non è possibile andare a destra
	return sp()->cur != sp()->end;
}
