#include "../internal.h"

// versione di rilascia_tab() per i programmi 'bare': usa lo heap
void rilascia_tab(paddr tab)
{
	dealloc(voidptr_cast(tab));
}
