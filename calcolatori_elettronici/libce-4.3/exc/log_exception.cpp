#include "../internal.h"

#ifdef __x86_64__
#define XIP "RIP"
#else
#define XIP "EIP"
#endif

void log_exception(int tipo, natq errore, vaddr xip)
{
	unsigned long long v, e, p;

	e = static_cast<unsigned long long>(errore);
	p = static_cast<unsigned long long>(xip);

	flog(LOG_WARN, "Eccezione %d (%s), errore %llx, " XIP " %#llx", tipo, eccezioni[tipo], e, p);
	// se l'eccezione fornisce un codice di errore, diamo ulteriori dettagli
	switch (tipo) {
	case 8:  // double fault
	case 10: // invalid TSS fault
	case 11: // segment fault
	case 12: // stack fault
	case 13: // protection fault
	case 17: // alignment check
		if (e & SE_EXT) {
			flog(LOG_WARN, "  errore dovuto ad un evento esterno");
			e &= ~SE_EXT;
		}
		// se l'errore, tolto ext, è completamente nullo, i bit idt e ti
		// non hanno significato
		if (e) {
			flog(LOG_WARN, "  errore all'offset %#2llx della %s", (e & ~0x7),
				e & SE_IDT ? "IDT":
				e & SE_TI  ? "LDT" : "GDT");
		}
		break;
	case 14:
		// page fault. Cerchiamo di dare più dettagli possibile.
		// In CR2 c'è l'indirizzo virtuale che ha causato il fault.
		v = static_cast<unsigned long long>(readCR2());
		flog(LOG_WARN, "  indirizzo virtuale: %llx %s", v,
			(v < DIM_PAGINA) ? "(probabile puntatore NULL)" : "");
		flog(LOG_WARN, "  dettagli: %s, %s, %s, %s",
			(errore & PF_PROT)  ? "protezione"	: "pag o tab assente",
			(errore & PF_WRITE) ? "scrittura"	: "lettura",
			(errore & PF_USER)  ? "da utente"	: "da sistema",
			(errore & PF_RES)   ? "bit riservato"	: "");
		break;
	default:
		// le altre eccezioni non forniscono codici di errore
		break;
	}
}
