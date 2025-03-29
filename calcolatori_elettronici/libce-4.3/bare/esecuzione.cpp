#include "../internal.h"

// finto descrittore di processo, usato dai programmi 'bare' diversi dal
// nucleo, in modo che flog() possa essere la stessa per tutti
struct des_proc {
	natw id;
} fake_process = { 0xFFFF };
struct des_proc *esecuzione = &fake_process;
