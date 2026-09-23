#include "../internal.h"

using namespace heap;

size_t disponibile()
{
        size_t tot = 0;
        for (des_mem *scan = memlibera; scan; scan = natq2desmem(scan->next))
                tot += scan->dimensione;
        return tot;
}
