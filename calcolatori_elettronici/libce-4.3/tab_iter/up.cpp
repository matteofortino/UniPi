#include "../internal.h"

bool tab_iter::up()
{
	if (done())
		return false;

	// per andare in alto ci limitiamo a incrementare l, tornando ad una
	// riga dello stack precedentemente utilizzata o a quella con ->tab==0,
	// che indica la fine della visita.
	l++;
	return true;
}
