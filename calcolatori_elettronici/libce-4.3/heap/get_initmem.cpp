#include "../internal.h"

using namespace heap;

natq heap_getinitmem()
{
	return desmem2natq(memlibera);
}
