#include <all.h>

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

#define ckwatch(ev_) do {\
	natq ev__ = static_cast<natq>(ev_);\
	natq v_ = watch();\
	if ((ev__) != v_) {\
		err("ottenuto %lx invece di %lx", v_, ev__);\
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
natl t00s0;

void t00p0b(natq test_num)
{
	watch();
	err("watch() senza setwatch() non ha causato abort");
	terminate_p();
}

void t00p1b(natq test_num)
{
	sem_wait(t00s0);
	watch();
	err("watch() da processo non owner non ha causato abort");
	terminate_p();
}

void t00p2b(natq test_num)
{
	setwatch(&t00p2, sizeof(t00p2));
	sem_signal(t00s0);
	terminate_p();
}

///**********************************************************************
// *             test 01: caso base                                     *
// **********************************************************************/

natl t01p0;
natl t01p1;
TCNT(01);
natq t01q0;

void t01p0b(natq test_num)
{
	t01q0 = 0x0011223344556677;
	setwatch(&t01q0, sizeof(t01q0));
	ckwatch(0x8899aabbccddeeff);
	testok(01);
	end_test();
}

void t01p1b(natq test_num)
{
	t01q0 = 0x8899aabbccddeeff;
	testok(01);
	end_test();
}

///**********************************************************************
// *             test 02: regione più piccola                           *
// **********************************************************************/

natl t02p0;
natl t02p1;
TCNT(02);
natb t02q0;

void t02p0b(natq test_num)
{
	t02q0 = 0x11;
	setwatch(&t02q0, sizeof(t02q0));
	ckwatch(0xaa);
	testok(02);
	end_test();
}

void t02p1b(natq test_num)
{
	t02q0 = 0xaa;
	testok(02);
	end_test();
}

///**********************************************************************
// *             test 03: sequenza di accessi                           *
// **********************************************************************/

natl t03p0;
natl t03p1;
TCNT(03);
natq t03q0;

void t03p0b(natq test_num)
{
	t03q0 = 0x1111111111111111;
	setwatch(&t03q0, sizeof(t03q0));
	ckwatch(0x2222222222222222);
	ckwatch(0x1111111111111111);
	testok(03);
	end_test();
}

void t03p1b(natq test_num)
{
	t03q0 = 0x2222222222222222;
	t03q0 = 0x1111111111111111;
	testok(03);
	end_test();
}

///**********************************************************************
// *             test 04: valore immutato                               *
// **********************************************************************/

natl t04p0;
natl t04p1;
TCNT(04);
natq t04q0;

void t04p0b(natq test_num)
{
	t04q0 = 0x1111111111111111;
	setwatch(&t04q0, sizeof(t04q0));
	ckwatch(0x2222222222222222);
	testok(04);
	end_test();
}

void t04p1b(natq test_num)
{
	t04q0 = 0x1111111111111111;
	t04q0 = 0x2222222222222222;
	testok(04);
	end_test();
}

///**********************************************************************
// *             test 05: più processi                                  *
// **********************************************************************/

natl t05p0;
natl t05p1;
natl t05p2;
TCNT(05);
natq t05q0;

void t05p0b(natq test_num)
{
	t05q0 = 0x1111111111111111;
	setwatch(&t05q0, sizeof(t05q0));
	delay(10);
	ckwatch(0x2222222222222222);
	ckwatch(0x3333333333333333);
	testok(05);
	end_test();
}

void t05p1b(natq test_num)
{
	delay(5);
	t05q0 = 0x3333333333333333;
	testok(05);
	end_test();
}

void t05p2b(natq test_num)
{
	t05q0 = 0x2222222222222222;
	testok(05);
	end_test();
}

///**********************************************************************
// *             test 06: accessi dal watcher                           *
// **********************************************************************/

natl t06p0;
natl t06p1;
TCNT(06);
natq t06q0;

void t06p0b(natq test_num)
{
	setwatch(&t06q0, sizeof(t06q0));
	t06q0 = 0x1111111111111111;
	ckwatch(0x2222222222222222);
	testok(06);
	end_test();
}

void t06p1b(natq test_num)
{
	natq q = t06q0;
	if (q != 0x1111111111111111) {
		err("letto %lx invece di 0x1111111111111111", q);
	}
	t06q0 = 0x2222222222222222;
	testok(06);
	end_test();
}

/**********************************************************************/


extern "C" void main()
{
	natl prio = 600;

	end_test = sem_ini(0);

	test_num = 0;
	dbg(">>>INIZIO<<<: errori vari");
	t00s0 = sem_ini(0);
	new_proc(00, 0);
	new_proc(00, 1);
	new_proc(00, 2);
	delay(1);
	dbg("=== FINE ===");

	test_num = 1;
	dbg(">>>INIZIO<<<: caso base");
	new_proc(01, 0);
	new_proc(01, 1);
	sem_wait(end_test);
	sem_wait(end_test);
	if (t01n0 == 2) msg("OK");
	dbg("=== FINE ===");

	test_num = 2;
	dbg(">>>INIZIO<<<: regione più piccola");
	new_proc(02, 0);
	new_proc(02, 1);
	sem_wait(end_test);
	sem_wait(end_test);
	if (t02n0 == 2) msg("OK");
	dbg("=== FINE ===");

	test_num = 3;
	dbg(">>>INIZIO<<<: sequenza di accessi");
	new_proc(03, 0);
	new_proc(03, 1);
	sem_wait(end_test);
	sem_wait(end_test);
	if (t03n0 == 2) msg("OK");
	dbg("=== FINE ===");

	test_num = 4;
	dbg(">>>INIZIO<<<: valore immutato");
	new_proc(04, 0);
	new_proc(04, 1);
	sem_wait(end_test);
	sem_wait(end_test);
	if (t04n0 == 2) msg("OK");
	dbg("=== FINE ===");

	test_num = 5;
	dbg(">>>INIZIO<<<: più processi");
	new_proc(05, 0);
	new_proc(05, 1);
	new_proc(05, 2);
	sem_wait(end_test);
	sem_wait(end_test);
	sem_wait(end_test);
	if (t05n0 == 3) msg("OK");
	dbg("=== FINE ===");

	test_num = 6;
	dbg(">>>INIZIO<<<: accessi dal watcher");
	new_proc(06, 0);
	new_proc(06, 1);
	sem_wait(end_test);
	sem_wait(end_test);
	if (t06n0 == 2) msg("OK");
	dbg("=== FINE ===");

	pause();

	terminate_p();
}
