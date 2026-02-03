#include <all.h>
#include <vm.h>

#define dbg(s, ...) flog(LOG_DEBUG, "TEST %lu: " s, test_num, ## __VA_ARGS__)
#define msg(s, ...) printf("TEST %lu PROC %d: " s "\n", test_num, getpid(), ## __VA_ARGS__)
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

#define chkr(r_, e_) do {\
	natl pf0_ = getmeminfo().pf_counter;\
	char c_ = (r_);\
	natl pf1_ = getmeminfo().pf_counter;\
	if (pf1_ != pf0_)\
		die("page fault durante una lettura");\
	if (c_ != (e_))\
		die("letto %02hhx invece di %02hhx", c_, e_);\
} while (0)

#define chkw(r_, c_, f_) do {\
	natl pf0_ = getmeminfo().pf_counter;\
	(r_) = (c_);\
	natl pf1_ = getmeminfo().pf_counter;\
	if (pf1_ - pf0_ != (f_))\
		die("generati %d page fault invece di %d", pf1_ - pf0_, (f_));\
} while (0)

natq test_num;

volatile char* cow = reinterpret_cast<char*>(norm(I_UTN_W * (1ULL << 39)));

///**********************************************************************
// *             test 00: errori vari                                   *
// **********************************************************************/

natl t00p0;
natl t00p1;

void t00p0b(natq test_num)
{
	char c = cow[DIM_USR_COW + DIM_PAGINA]; (void)c;
	err("accesso fuori dalla zona cow non ha causato abort");
	terminate_p();
}

void t00p1b(natq test_num)
{
	char c = cow[DIM_USR_COW]; (void)c;
	err("accesso subito fuori dalla zona cow non ha causato abort");
	terminate_p();
}

///**********************************************************************
// *             test 01: lettura di un byte                            *
// **********************************************************************/

natl t01p0;
TCNT(01);

void t01p0b(natq test_num)
{
	chkr(cow[DIM_USR_COW/2], 0);
	testok(01);
	end_test();
}

///**********************************************************************
// *             test 02: scrittura di un byte                          *
// **********************************************************************/

natl t02p0;
TCNT(02);

void t02p0b(natq test_num)
{
	chkr(cow[DIM_USR_COW/2], 0);
	chkw(cow[DIM_USR_COW/2], 'a', 1);
	chkr(cow[DIM_USR_COW/2], 'a');
	chkr(cow[DIM_USR_COW/2+1], 0);
	testok(02);
	end_test();
}

///**********************************************************************
// *             test 03: lettura di un byte ereditato                  *
// **********************************************************************/

natl t03p0;
natl t03p1;
natl t03s0;
TCNT(03);

void t03p1b(natq test_num)
{
	chkr(cow[1234], 'b');
	sem_signal(t03s0);
	testok(03);
	end_test();
}

void t03p0b(natq test_num)
{
	chkr(cow[1234], 0);
	chkw(cow[1234], 'b', 1);
	chkr(cow[1234], 'b');
	activate_p(t03p1b, test_num, 300, LIV_UTENTE);
	sem_wait(t03s0);
	testok(03);
	end_test();
}

///**********************************************************************
// *             test 04: scrittura dopo la copia                       *
// **********************************************************************/

natl t04p0;
natl t04p1;
natl t04s0;
TCNT(04);

void t04p1b(natq test_num)
{
	chkr(cow[1234], 0);
	chkw(cow[1234], 'd', 1);
	sem_signal(t04s0);
	testok(04);
	end_test();
}

void t04p0b(natq test_num)
{
	chkr(cow[1234], 0);
	activate_p(t04p1b, test_num, 299, LIV_UTENTE);
	chkw(cow[1234], 'c', 1);
	sem_wait(t04s0);
	chkr(cow[1234], 'c');
	testok(04);
	end_test();
}

///**********************************************************************
// *             test 05: scrittura prima e dopo la copia               *
// **********************************************************************/

natl t05p0;
natl t05p1;
natl t05s0;
TCNT(05);

void t05p1b(natq test_num)
{
	chkr(cow[1234], 'x');
	chkw(cow[1234], 'z', 1);
	sem_signal(t05s0);
	testok(05);
	end_test();
}

void t05p0b(natq test_num)
{
	chkr(cow[1234], 0);
	chkw(cow[1234], 'x', 1);
	activate_p(t05p1b, test_num, 298, LIV_UTENTE);
	chkw(cow[1234], 'y', 1);
	sem_wait(t05s0);
	chkr(cow[1234], 'y');
	testok(05);
	end_test();
}

///**********************************************************************
// *             test 06: copia della copia                             *
// **********************************************************************/

natl t06p0;
natl t06p1;
natl t06p2;
natl t06s0;
natl t06s1;
TCNT(06);

void t06p2b(natq test_num)
{
	chkr(cow[1234], 'W');
	chkw(cow[1234], 'A', 1);
	sem_signal(t06s1);
	testok(06);
	end_test();
}

void t06p1b(natq test_num)
{
	chkr(cow[1234], 'X');
	chkw(cow[1234], 'W', 1);
	activate_p(t06p2b, test_num, 296, LIV_UTENTE);
	chkw(cow[1234], 'Z', 1);
	sem_wait(t06s1);
	chkr(cow[1234], 'Z');
	sem_signal(t06s0);
	testok(06);
	end_test();
}

void t06p0b(natq test_num)
{
	chkr(cow[1234], 0);
	chkw(cow[1234], 'X', 1);
	activate_p(t06p1b, test_num, 297, LIV_UTENTE);
	chkw(cow[1234], 'Y', 1);
	sem_wait(t06s0);
	chkr(cow[1234], 'Y');
	testok(06);
	end_test();
}


/**********************************************************************/


extern natl mainp;
void main_body(natq id)
{
	natl prio = 600;
	natl membefore = getmeminfo().num_frame_liberi;

	end_test = sem_ini(0);

	test_num = 0;
	dbg(">>>INIZIO<<<: errori vari");
	new_proc(00, 0);
	delay(1);
	new_proc(00, 1);
	delay(1);
	dbg("=== FINE ===");

	test_num = 1;
	dbg(">>>INIZIO<<<: lettura di un byte");
	t01m0 = sem_ini(1);
	new_proc(01, 0);
	sem_wait(end_test);
	if (t01n0 == 1) msg("OK");
	dbg("=== FINE ===");

	test_num = 2;
	dbg(">>>INIZIO<<<: scrittura di un byte");
	t02m0 = sem_ini(1);
	new_proc(02, 0);
	sem_wait(end_test);
	if (t02n0 == 1) msg("OK");
	dbg("=== FINE ===");

	test_num = 3;
	dbg(">>>INIZIO<<<: lettura di un byte ereditato");
	t03m0 = sem_ini(1);
	t03s0 = sem_ini(0);
	new_proc(03, 0);
	sem_wait(end_test);
	sem_wait(end_test);
	if (t03n0 == 2) msg("OK");
	dbg("=== FINE ===");

	test_num = 4;
	dbg(">>>INIZIO<<<: scrittura dopo la copia");
	t04m0 = sem_ini(1);
	t04s0 = sem_ini(0);
	new_proc(04, 0);
	sem_wait(end_test);
	sem_wait(end_test);
	if (t04n0 == 2) msg("OK");
	dbg("=== FINE ===");

	test_num = 5;
	dbg(">>>INIZIO<<<: scrittura prima e dopo la copia");
	t05m0 = sem_ini(1);
	t05s0 = sem_ini(0);
	new_proc(05, 0);
	sem_wait(end_test);
	sem_wait(end_test);
	if (t05n0 == 2) msg("OK");
	dbg("=== FINE ===");

	test_num = 6;
	dbg(">>>INIZIO<<<: copia della copia");
	t06m0 = sem_ini(1);
	t06s0 = sem_ini(0);
	t06s1 = sem_ini(0);
	new_proc(06, 0);
	sem_wait(end_test);
	sem_wait(end_test);
	sem_wait(end_test);
	if (t06n0 == 3) msg("OK");
	dbg("=== FINE ===");

	delay(10);
	test_num = 6;
	natl memafter = getmeminfo().num_frame_liberi;
	if (memafter != membefore)
		err("memoria non liberata: %d frame", membefore - memafter);
	pause();

	terminate_p();
}
natl mainp;

extern "C" void main()
{
	mainp = activate_p(main_body, 0, 900, LIV_UTENTE);

	terminate_p();
}
