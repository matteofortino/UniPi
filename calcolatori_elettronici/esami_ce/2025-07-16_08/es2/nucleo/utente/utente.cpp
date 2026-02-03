#include <all.h>
#include <vm.h>

#define dbg(s, ...) flog(LOG_DEBUG, "TEST %lu: " s, test_num, ## __VA_ARGS__)
#define msg(s, ...) printf("TEST %lu PROC %u: " s "\n", test_num, getpid(), ## __VA_ARGS__)
#define err(s, ...) msg("ERRORE: " s, ## __VA_ARGS__)
#define die(s, ...) do { err(s, ## __VA_ARGS__); goto error; } while (0)

#define new_proc(tn, pn)\
	t##tn##p##pn = activate_p(t##tn##p##pn##b, test_num, prio--, LIV_UTENTE);

natl end_test; // sync

#define end_subtest() do {\
	(void)&&error;\
error:\
	terminate_p();\
} while (0)

#define end_test() do {\
	(void)&&error;\
error:\
	sem_signal(end_test);\
	terminate_p();\
} while (0)

#define TCNT(n)	natl t##n##m0; natl t##n##n0;
#define testok(n) do {\
	sem_wait(t##n##m0);\
	t##n##n0++;\
	sem_signal(t##n##m0);\
} while (0)

#define sloterr(s_)  ((s_) == 0xFFFFFFFF ? "0xFFFFFFFF" : "uno slot")

#define ckopen(p_, w_, es_) ({\
	natl s_ = openpipe(p_, w_);\
	if (((es_) == 0xFFFFFFFF && s_ != 0xFFFFFFFF) || ((es_) != 0xFFFFFFFF && s_ == 0xFFFFFFFF) ) {\
		err("openpipe: ottenuto %s invece di %s", sloterr(s_), sloterr(es_));\
		goto error;\
	}\
	s_;\
})

#define bool2str(r_) ((r_) ? "true" : "false")

#define cktrns(d_, s_, b_, n_, er_) do {\
	bool r_ = d_##pipe(s_, b_, n_);\
	if ((er_) != r_) {\
		err(#d_ ": ottenuto %s (%x) invece di %s", bool2str(r_), r_, bool2str(er_));\
		goto error;\
	}\
} while (0)

natq test_num;
char dummy[1*MiB+360*KiB];
char stacks[4][DIM_USR_STACK];

///**********************************************************************
// *             test 00: errori vari                                   *
// **********************************************************************/

natl t00p0;

void t00p0b(natq test_num)
{
	asm volatile("\txor %%rdi, %%rdi; int %0" : : "i" (TIPO_RES) : "di", "memory");
	err("resident() invocata da livello utente non ha causato abort");
	terminate_p();
}

///**********************************************************************
// *             test 01: caso base                                     *
// **********************************************************************/

natl t01p0;
TCNT(01);

void t01p0b(natq test_num)
{
	const int nb = 2;
	const int sz = nb * 512;
	char buf[sz];
	memset(buf, '@', sz);
	dmawritehd_n(buf, 0, nb);
	memset(buf, 0, sz);
	dmareadhd_n(buf, 0, nb);
	for (int i = 0; i < sz; i++) {
		if (buf[i] != '@') {
			err("byte %d: atteso %x, letto %x", i, '@', buf[i]);
		}
	}
	testok(01);
	end_test();
}

///**********************************************************************
// *             test 02: dma dopo swap_out                             *
// **********************************************************************/

natl t02p0;
natl t02s0;
TCNT(02);

void t02p0b(natq test_num)
{
	const int np = 3;
	const int nb = np * 8;
	const int sz = nb * 512;
	char buf[sz];
	for (int i = 0; i < np; i++)
		memset(buf + i * DIM_PAGINA, 'A' + i, DIM_PAGINA);
	sem_signal(t02s0);
	dmawritehd_n(buf, 0, nb);
	memset(buf, 0, sz);
	dmareadhd_n(buf, 0, nb);
	for (int j = 0; j < np; j++) {
		for (natq i = 0; i < DIM_PAGINA; i++) {
			if (buf[j * DIM_PAGINA + i] != 'A' + j) {
				err("byte %llu: atteso %x, letto %x", j * DIM_PAGINA + i, 'A' + j, buf[i]);
			}
		}
	}
	sem_signal(t02s0);
	testok(02);
	end_test();
}

///**********************************************************************
// *             test 03: swap_out durante dma                          *
// **********************************************************************/

const int t03c0 = 3;
const int t03c1 = t03c0 * 8;
const int t03c2 = t03c1 * 512;
char save[t03c2];
natl t03p0;
natl t03s0;
TCNT(03);

void t03p0b(natq test_num)
{
	char buf[t03c2];
	for (int i = 0; i < t03c0; i++)
		memset(save + i * DIM_PAGINA, 'D' + i, DIM_PAGINA);
	save[0] = 's';
	memcpy(save + 1, &t03s0, sizeof(t03s0));
	memcpy(buf, save, t03c2);
	dmawritehd_n(buf, 0, t03c1);
	memset(buf, 0, t03c2);
	dmareadhd_n(buf, 0, t03c1);
	for (int i = 0; i < t03c2; i++) {
		if (buf[i] != save[i]) {
			err("byte %d: atteso %x, letto %x", i, save[i], buf[i]);
		}
	}
	testok(03);
	end_test();
}


///**********************************************************************
// *             test 04: rimozione dopo dma                            *
// **********************************************************************/

natl t04p0;
natl t04s0;
TCNT(04);

void t04p0b(natq test_num)
{
	const int np = 3;
	const int nb = np * 8;
	const int sz = nb * 512;
	char buf[sz];
	for (int i = 0; i < np; i++)
		memset(buf + i * DIM_PAGINA, 'G' + i, DIM_PAGINA);
	dmawritehd_n(buf, 0, nb);
	memset(buf, 0, sz);
	dmareadhd_n(buf, 0, nb);
	sem_signal(t04s0);
	for (int j = 0; j < np; j++) {
		for (natq i = 0; i < DIM_PAGINA; i++) {
			if (buf[j * DIM_PAGINA + i] != 'G' + j) {
				err("byte %llu: atteso %x, letto %x", j * DIM_PAGINA + i, 'A' + j, buf[i]);
			}
		}
	}
	testok(04);
	end_test();
}

/**********************************************************************/

natl nexttest;
natl acktest;
void myswapper(natq nexttest)
{
	natl rv;
	swapper();
	sem_wait(nexttest);
	// test 2
	sem_wait(t02s0);
	swap_out(t02p0, stacks[0]);
	if ( (rv = get_swap_ev()) != t02p0) {
		msg("get_swap_ev(): atteso %u, ricevuto %u", t02p0, rv);
		goto out;
	}
	if ( (rv = swap_in(t02p0, stacks[0])) != true) {
		msg("swap_in(): atteso true(%x), ricevuto %x", true, rv);
		goto out;
	}
	sem_wait(t02s0);
	sem_signal(acktest);

	sem_wait(nexttest);
	// test 3
	sem_wait(t03s0);
	swap_out(t03p0, stacks[0]);
	if ( (rv = get_swap_ev()) != t03p0) {
		msg("get_swap_ev(): atteso %u, ricevuto %u", t03p0, rv);
		goto out;
	}
	if ( (rv = swap_in(t03p0, stacks[0])) != true) {
		msg("swap_in(): atteso true(%x), ricevuto %x", true, rv);
		goto out;
	}
	sem_signal(acktest);

	sem_wait(nexttest);
	// test 4
	sem_wait(t04s0);
	swap_out(t04p0, stacks[0]);
	if ( (rv = get_swap_ev()) != t04p0) {
		msg("get_swap_ev(): atteso %u, ricevuto %u", t04p0, rv);
		goto out;
	}
	if ( (rv = swap_in(t04p0, stacks[0])) != true) {
		msg("swap_in(): atteso true(%x), ricevuto %x", true, rv);
		goto out;
	}
	sem_signal(acktest);
out:
	terminate_p();
}

extern natl mainp;
void main_body(natq id)
{
	natl prio = 600;

	nexttest = sem_ini(0);
	acktest = sem_ini(0);
	end_test = sem_ini(0);

	test_num = 0;
	dbg(">>>INIZIO<<<: errori vari");
	new_proc(00, 0);
	delay(10);
	dbg("=== FINE ===");

	test_num = 1;
	dbg(">>>INIZIO<<<: caso base");
	new_proc(01, 0);
	sem_wait(end_test);
	if (t01n0 == 1) msg("OK");
	dbg("=== FINE ===");

	if (activate_p(myswapper, nexttest, 700, LIV_UTENTE) == 0xFFFFFFFF) {
		msg("Impossibile creare il processo swapper");
		pause();
		terminate_p();
	}

	test_num = 2;
	dbg(">>>INIZIO<<<: dma dopo rimozione");
	t02s0 = sem_ini(0);
	new_proc(02, 0);
	sem_signal(nexttest);
	sem_wait(acktest);
	sem_wait(end_test);
	if (t02n0 == 1) msg("OK");
	delay(10);
	dbg("=== FINE ===");

	test_num = 3;
	dbg(">>>inizio<<<: rimozione durante dma");
	t03s0 = sem_ini(0);
	new_proc(03, 0);
	sem_signal(nexttest);
	sem_wait(acktest);
	sem_wait(end_test);
	if (t03n0 == 1) msg("OK");
	delay(10);
	dbg("=== fine ===");

	test_num = 4;
	dbg(">>>inizio<<<: rimozione dopo dma");
	new_proc(04, 0);
	t04s0 = sem_ini(0);
	sem_signal(nexttest);
	sem_wait(acktest);
	sem_wait(end_test);
	if (t04n0 == 1) msg("OK");
	delay(10);
	dbg("=== fine ===");

	sem_signal(nexttest);
	pause();
	terminate_p();
}
natl mainp;

extern "C" void main()
{
	mainp = activate_p(main_body, 0, 900, LIV_UTENTE);

	terminate_p();
}
