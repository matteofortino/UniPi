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

///**********************************************************************
// *             test 00: errori vari                                   *
// **********************************************************************/

natl t00p0;
natl t00p1;
natl t00p2;
natl t00p3;
natl t00p4;
natl t00p5;
natl t00s0;
char t00v0[DIM_USR_STACK];
const char t00v1[DIM_USR_STACK] = { 0 };

void t00p0b(natq test_num)
{
	swap_out(getpid(), t00v0);
	err("swap_out() sul processo invocante non ha causato abort\n");
	terminate_p();
}

void t00p1b(natq test_num)
{
	swap_out(0, t00v0);
	swap_in(0, t00v0);
	err("swap_out() o swap_in() su processo di sistema non ha causato abort\n");
	terminate_p();
}

void t00p2b(natq test_num)
{
	swap_out(MAX_PROC_ID + 1, t00v0);
	swap_in(MAX_PROC_ID, t00v0);
	err("swap_out() o swap_in() su pid non valido non ha causato abort\n");
	terminate_p();
}

void t00p3b(natq test_num)
{
	sem_wait(t00s0);
	terminate_p();
}

void t00p4b(natq test_num)
{
	swap_out(t00p3, const_cast<char*>(t00v1));
	err("swap_out() con buffer non scrivibile non ha causato abort");
	terminate_p();
}

void t00p5b(natq test_num)
{
	if (swap_in(t00p3, t00v1)) {
		err("swap_in() di processo non rimosso ha restituito true");
		terminate_p();
	}
	sem_signal(t00s0);
	terminate_p();
}

///**********************************************************************
// *             test 01: caso base                                     *
// **********************************************************************/

natl t01p0;
natl t01p1;
natl t01p2;
natl t01s0;
char t01v0[DIM_USR_STACK];
TCNT(01);

void t01p0b(natq test_num)
{
	volatile natq i = 0xaabbccddeeff;
	sem_wait(t01s0);
	if (i != 0xaabbccddeeff) {
		die("atteso 0xaabbccddeeff, ottenuto %08lx", i);
	}
	testok(01);
	end_test();
}

void t01p1b(natq test_num)
{
	natl i;
	swap_out(t01p0, t01v0);
	for (i = 0; i < DIM_USR_STACK; i += sizeof(natq)) {
		natq *p = reinterpret_cast<natq*>(&t01v0[i]);
		if (*p == 0xaabbccddeeff)
			break;
	}
	if (i >= DIM_USR_STACK) {
		die("valore 0xaabbccddeeff non trovato");
	}
	if (!swap_in(t01p0, t01v0)) {
		die("swap_in() ha restituito false");
	}
	sem_signal(t01s0);
	testok(01);
	end_test();
}

///**********************************************************************
// *             test 02: schedulazione di un processo rimosso          *
// **********************************************************************/

natl t02p0;
natl t02p1;
natl t02p2;
natl t02s0;
char t02v0[DIM_USR_STACK];
TCNT(02);

void t02p0b(natq test_num)
{
	volatile natq i = 0x11223344556677;
	sem_wait(t02s0);
	if (i != 0x11223344556677) {
		die("atteso 0x11223344556677, ottenuto %08lx", i);
	}
	testok(02);
	end_test();
}

void t02p1b(natq test_num)
{
	natl i;
	swap_out(t02p0, t02v0);
	for (i = 0; i < DIM_USR_STACK; i += sizeof(natq)) {
		natq *p = reinterpret_cast<natq*>(&t02v0[i]);
		if (*p == 0x11223344556677)
			break;
	}
	if (i >= DIM_USR_STACK) {
		die("valore 0x11223344556677 non trovato");
	}
	sem_signal(t02s0);
	if (!swap_in(t02p0, t02v0)) {
		die("swap_in() ha restituito false");
	}
	testok(02);
	end_test();
}

///**********************************************************************
// *             test 03: rispetto delle precedenze                     *
// **********************************************************************/

natl t03p0;
natl t03p1;
natl t03p2;
natl t03p3;
natl t03s0;
natl t03s1;
natl t03s2;
natl t03s3;
natl t03v0;
char t03v1[DIM_USR_STACK];
char t03v2[DIM_USR_STACK];
TCNT(03);

void t03p0b(natq test_num)
{
	natl i;
	sem_wait(t03s0);
	swap_out(t03p1, t03v1);
	sem_wait(t03s1);
	swap_out(t03p2, t03v2);
	for (i = 0; i < DIM_USR_STACK; i += sizeof(natq)) {
		natq *p = reinterpret_cast<natq*>(&t03v1[i]);
		if (*p == 0x77665544332211)
			break;
	}
	if (i >= DIM_USR_STACK) {
		die("valore 0x77665544332211 non trovato");
	}
	for (i = 0; i < DIM_USR_STACK; i += sizeof(natq)) {
		natq *p = reinterpret_cast<natq*>(&t03v2[i]);
		if (*p == 0xffeeddccbbaa)
			break;
	}
	if (i >= DIM_USR_STACK) {
		die("valore 0xffeeddccbbaa non trovato");
	}
	sem_wait(t03s2);
	if (!swap_in(t03p1, t03v1)) {
		die("swap_in() ha restituito false");
	}
	if (!swap_in(t03p2, t03v2)) {
		die("swap_in() ha restituito false");
	}
	testok(03);
	end_test();
}

void t03p1b(natq test_num)
{
	volatile natq i = 0x77665544332211;
	sem_signal(t03s0);
	if (i != 0x77665544332211) {
		die("atteso 0x77665544332211, ottenuto %08lx", i);
	}
	if (t03v0 != 0) {
		die("errore di schedulazione (atteso 0, trovato %u)", t03v0);
	}
	t03v0 = 1;
	testok(03);
	end_test();
}

void t03p2b(natq test_num)
{
	volatile natq i = 0xffeeddccbbaa;
	sem_signal(t03s1);
	if (i != 0xffeeddccbbaa) {
		die("atteso 0xffeeddccbbaa, ottenuto %08lx", i);
	}
	if (t03v0 != 1) {
		die("errore di schedulazione (atteso 1, trovato %u)", t03v0);
	}
	t03v0 = 2;
	testok(03);
	end_test();
}

void t03p3b(natq test_num)
{
	sem_signal(t03s2);
	if (t03v0 != 2) {
		die("errore di schedulazione (atteso 2, trovato %u)", t03v0);
	}
	testok(03);
	end_test();
}

///**********************************************************************
// *             test 04: rispetto delle precedenze (2)                 *
// **********************************************************************/

natl t04p0;
natl t04p1;
natl t04p2;
natl t04p3;
natl t04s0;
natl t04s1;
natl t04s2;
natl t04s3;
char t04v0[DIM_USR_STACK];
char t04v1[DIM_USR_STACK];
natl t04v2;
TCNT(04);

void t04p0b(natq test_num)
{
	volatile natq i = 0x77656544332211;
	sem_wait(t04s0);
	if (i != 0x77656544332211) {
		die("atteso 0x77656544332211, ottenuto %08lx", i);
	}
	if (t04v2 != 2) {
		die("errore di schedulazione (atteso 2, trovato %u)", t04v2);
	}
	t04v2 = 3;
	testok(04);
	end_test();
}

void t04p1b(natq test_num)
{
	volatile natq i = 0xffeedcdcbbaa;
	sem_wait(t04s1);
	if (i != 0xffeedcdcbbaa) {
		die("atteso 0xffeedcdcbbaa, ottenuto %08lx", i);
	}
	if (t04v2 != 0) {
		die("errore di schedulazione (atteso 0, trovato %u)", t04v2);
	}
	t04v2 = 1;
	testok(04);
	end_test();
}

void t04p2b(natq test_num)
{
	natl i;
	swap_out(t04p0, t04v0);
	sem_signal(t04s0);
	swap_out(t04p1, t04v1);
	sem_signal(t04s1);
	for (i = 0; i < DIM_USR_STACK; i += sizeof(natq)) {
		natq *p = reinterpret_cast<natq*>(&t04v0[i]);
		if (*p == 0x77656544332211)
			break;
	}
	if (i >= DIM_USR_STACK) {
		die("valore 0x77656544332211 non trovato");
	}
	for (i = 0; i < DIM_USR_STACK; i += sizeof(natq)) {
		natq *p = reinterpret_cast<natq*>(&t04v1[i]);
		if (*p == 0xffeedcdcbbaa)
			break;
	}
	if (i >= DIM_USR_STACK) {
		die("valore 0xffeedcdcbbaa non trovato");
	}
	if (!swap_in(t04p1, t04v1)) {
		die("swap_in() ha restituito false");
	}
	if (t04v2 != 1) {
		die("errore di schedulazione (atteso 1, trovato %u)", t04v2);
	}
	t04v2 = 2;
	if (!swap_in(t04p0, t04v0)) {
		die("swap_in() ha restituito false");
	}
	if (t04v2 != 3) {
		die("errore di schedulazione (atteso 3, trovato %u)", t04v2);
	}
	testok(04);
	end_test();
}


/**********************************************************************/

extern natl mainp;
void main_body(natq id)
{
	natl prio = 600;

	end_test = sem_ini(0);

	test_num = 0;
	dbg(">>>INIZIO<<<: errori vari");
	t00s0 = sem_ini(0);
	new_proc(00, 0);
	new_proc(00, 1);
	new_proc(00, 2);
	new_proc(00, 3);
	new_proc(00, 4);
	new_proc(00, 5);
	delay(1);
	dbg("=== FINE ===");

	test_num = 1;
	dbg(">>>INIZIO<<<: caso base");
	t01s0 = sem_ini(0);
	new_proc(01, 0);
	new_proc(01, 1);
	sem_wait(end_test);
	sem_wait(end_test);
	if (t01n0 == 2) msg("OK");
	dbg("=== FINE ===");

	test_num = 2;
	dbg(">>>INIZIO<<<: schedulazione di un processo rimosso");
	t02s0 = sem_ini(0);
	new_proc(02, 0);
	new_proc(02, 1);
	sem_wait(end_test);
	sem_wait(end_test);
	if (t02n0 == 2) msg("OK");
	dbg("=== FINE ===");

	test_num = 3;
	dbg(">>>inizio<<<: rispetto delle precedenze");
	t03s0 = sem_ini(0);
	t03s1 = sem_ini(0);
	t03s2 = sem_ini(0);
	t03s3 = sem_ini(0);
	new_proc(03, 0);
	new_proc(03, 1);
	new_proc(03, 2);
	new_proc(03, 3);
	sem_wait(end_test);
	sem_wait(end_test);
	sem_wait(end_test);
	sem_wait(end_test);
	if (t03n0 == 4) msg("ok");
	dbg("=== fine ===");

	test_num = 4;
	dbg(">>>inizio<<<: rispetto delle precedenze (2)");
	t04s0 = sem_ini(0);
	t04s1 = sem_ini(0);
	t04s2 = sem_ini(0);
	new_proc(04, 0);
	new_proc(04, 1);
	new_proc(04, 2);
	sem_wait(end_test);
	sem_wait(end_test);
	sem_wait(end_test);
	if (t04n0 == 3) msg("ok");
	dbg("=== fine ===");

	pause();

	terminate_p();
}
natl mainp;

extern "C" void main()
{
	mainp = activate_p(main_body, 0, 900, LIV_UTENTE);

	terminate_p();
}
