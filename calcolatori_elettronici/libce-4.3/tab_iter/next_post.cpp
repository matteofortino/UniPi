#include "../internal.h"

void tab_iter::next_post()
{
	if (done())
		return;

	// se è possibile andare a destra, ci andiamo e poi scendiamo fino alla
	// prima foglia seguendo i rami di sinistra (si veda 'post()')
	if (right()) {
		post();
	} else {
		// altrimenti risaliamo
		up();
	}
}
