#include "../internal.h"

extern "C" void gestore_eccezioni(unsigned long tipo, natq err, void* eip)
{
        log_exception(tipo, err, int_cast<vaddr>(eip));
	panic("Errore fatale");
}
