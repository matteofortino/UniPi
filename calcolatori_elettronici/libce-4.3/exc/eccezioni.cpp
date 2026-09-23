#include "../internal.h"

const char* eccezioni[] = {
	"errore di divisione", 		// 0
	"debug",			// 1
	"interrupt non mascherabile",	// 2
	"breakpoint",			// 3
	"overflow",			// 4
	"bound check",			// 5
	"codice operativo non valido",	// 6
	"dispositivo non disponibile",	// 7
	"doppio fault",			// 8
	"coprocessor segment overrun",	// 9
	"TSS non valido",		// 10
	"segmento non presente",	// 11
	"errore sul segmento stack",	// 12
	"errore di protezione",		// 13
	"page fault",			// 14
	"riservato",			// 15
	"errore su virgola mobile",	// 16
	"errore di allineamento",	// 17
	"errore interno",		// 18
	"errore SIMD",			// 19
	"errore di virtualizzazione",   // 20
	"riservato",			// 21
	"riservato",			// 22
	"riservato",			// 23
	"riservato",			// 24
	"riservato",			// 25
	"riservato",			// 26
	"riservato",			// 27
	"riservato",			// 28
	"riservato",			// 29
	"eccezione di sicurezza",	// 30
	"riservato"			// 31
};
