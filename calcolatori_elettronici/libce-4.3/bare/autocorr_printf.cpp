#include "../internal.h"

#define BUFSZ 1024
// versione di printf usata quando AUTOCORR=1. Invia l'output sul log invece
// che sul video, in modo che il sito dell'autocorrezione possa catturarlo più
// semplicemente.
int autocorr_printf(const char *fmt, ...)
{
        va_list ap;
        static char buf[BUFSZ];
	static char *p;
        int l, i;

        va_start(ap, fmt);
        l = vsnprintf(buf, BUFSZ, fmt, ap);
        va_end(ap);

	for (p = buf, i = 0; i < l; i++) {
		// se l'output è composto di più righe inviamo al log messaggi
		// separati
		if (buf[i] == '\n')
			buf[i] = '\0';
		if (buf[i] == '\0') {
			flog(LOG_USR, "%s", p);
			p = &buf[i + 1];
		}
	}

        return l;
}
