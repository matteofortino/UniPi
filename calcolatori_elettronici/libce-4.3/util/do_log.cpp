#include "../internal.h"
extern int MAX_LOG;
struct des_proc {
	natw id;
};
extern des_proc *esecuzione;
extern "C" void do_log(log_sev sev, const char* buf, natl quanti)
{
	const char* lev[] = { "DBG", "INF", "WRN", "ERR", "USR" };
	if (sev > MAX_LOG) {
		flog(LOG_WARN, "Livello di log errato: %d", sev);
		sev = LOG_INFO;
	}
	const char* l = lev[sev];
	while (*l)
		serial::out1(*l++);
	serial::out1('\t');
	char idbuf[10];
	if (!esecuzione) {
		serial::out1('?');
	} else if (esecuzione->id == 0xFFFF) {
		serial::out1('-');
	} else {
		snprintf(idbuf, 10, "%d", esecuzione->id);
		l = idbuf;
		while (*l)
			serial::out1(*l++);
	}
	serial::out1('\t');

	for (natl i = 0; i < quanti; i++)
		serial::out1(buf[i]);
	serial::out1('\n');
}
