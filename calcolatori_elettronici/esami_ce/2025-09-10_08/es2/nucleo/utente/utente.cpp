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

#define sloterr(s_)  ((s_) == 0xFFFFFFFF ? "0xFFFFFFFF" : "un id")

#define ckasyncread(i_, b_, n_, e_) ({\
	natl t_ = ceasyncread_n(i_, b_, n_);\
	if (((e_) == 0xFFFFFFFF && t_ != 0xFFFFFFFF) || ((e_) != 0xFFFFFFFF && t_ == 0xFFFFFFFF) ) {\
		err("ceasyncread_n: ottenuto %s invece di %s", sloterr(t_), sloterr(e_));\
		goto error;\
	}\
	if (t_ != 0xFFFFFFFF && t_ >= MAX_CE_ASYNC) {\
		err("ceasyncread_n: ottenuto %u fuori dall'intervallo [0, MAX_CE_ASYNC)", t_);\
	}\
	t_;\
})

#define ckwait(i_, t_, b1_, b2_, n_) do {\
	cewait(i_, t_);\
	for (int j_ = 0; j_ < (n_); j_++) {\
		if ((b1_)[j_] != (b2_)[j_]) {\
			err("byte %u: atteso '%c' (%x), ricevuto '%c' (%x)",\
					j_, (b2_)[j_], (b2_)[j_], (b1_)[j_], (b1_)[j_]);\
			goto error;\
		}\
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
char t00v0[10];
const char t00v1[10] =  { 0 };

void t00p0b(natq test_num)
{
	ceasyncread_n(2, t00v0, 10);
	err("ceasyncread_n su periferica non esistente non ha causato abort");
	terminate_p();
}

void t00p1b(natq test_num)
{
	ceasyncread_n(0, t00v0, 0);
	err("ceasyncread_n con argomenti non validi non ha causato abort");
	terminate_p();
}

void t00p2b(natq test_num)
{
	ceasyncread_n(0, nullptr, 10);
	err("ceasyncread_n con argomenti non validi non ha causato abort");
	terminate_p();
}

void t00p3b(natq test_num)
{
	ceasyncread_n(0, const_cast<char*>(t00v1), 10);
	err("ceasyncread_n con argomenti non validi non ha causato abort");
	terminate_p();
}

void t00p4b(natq test_num)
{
	char buf[10];
	ceasyncread_n(0, buf, 10);
	err("ceasyncread_n con argomenti non validi non ha causato abort");
	terminate_p();
}

///**********************************************************************
// *             test 01: caso base                                     *
// **********************************************************************/

natl t01p0;
char t01v0[10];
TCNT(01);

void t01p0b(natq test_num)
{
	natl t = ckasyncread(0, t01v0, 10, 0);
	ckwait(0, t, t01v0, "ABCDEFGHIJ", 10);
	testok(01);
	end_test();
}

///**********************************************************************
// *             test 02: due pendenti                                  *
// **********************************************************************/

natl t02p0;
char t02v0[10];
char t02v1[6];
TCNT(02);

void t02p0b(natq test_num)
{
	natl t1, t2;

	t1 = ckasyncread(0, t02v0, 10, 0);
	t2 = ckasyncread(0, t02v1, 6, 0);
	ckwait(0, t2, t02v1, "UVWXYZ", 6);
	ckwait(0, t1, t02v0, "KLMNOPQRST", 10);
	testok(02);
	end_test();
}

///**********************************************************************
// *             test 03: troppe richieste                              *
// **********************************************************************/

natl t03p0;
char t03v0[2];
char t03v1[2];
char t03v2[2];
char t03v3[2];
char t03v4[2];
TCNT(03);

void t03p0b(natq test_num)
{
	natl t0, t1, t2, t3, t4;

	t0 = ckasyncread(0, t03v0, 2, 0);
	t1 = ckasyncread(0, t03v1, 2, 0);
	t2 = ckasyncread(0, t03v2, 2, 0);
	t3 = ckasyncread(0, t03v3, 2, 0);
	ckasyncread(0, t03v1, 2, 0xFFFFFFFF);
	ckwait(0, t2, t03v2, "EF", 2);
	t4 = ckasyncread(0, t03v4, 2, 0);
	ckwait(0, t1, t03v1, "CD", 2);
	ckwait(0, t0, t03v0, "AB", 2);
	ckwait(0, t4, t03v4, "IJ", 2);
	ckwait(0, t3, t03v3, "GH", 2);
	testok(03);
	end_test();
}

///**********************************************************************
// *             test 04: più processi                                  *
// **********************************************************************/

natl t04p0;
natl t04p1;
char t04v0[20];
char t04v1[22];
TCNT(04);

void t04p0b(natq test_num)
{
	natl t = ckasyncread(0, t04v0, 20, 0);
	delay(5);
	ckwait(0, t, t04v0, "KLMNOPQRSTUVWXYZABCD", 20);
	testok(04);
	end_test();
}

void t04p1b(natq test_num)
{
	natl t = ckasyncread(0, t04v1, 22, 0);
	ckwait(0, t, t04v1, "EFGHIJKLMNOPQRSTUVWXYZ", 22);
	testok(04);
	end_test();
}

///**********************************************************************
// *             test 05: più periferiche                               *
// **********************************************************************/

natl t05p0;
natl t05p1;
char t05v0[19];
char t05v1[19];
TCNT(05);

void t05p0b(natq test_num)
{
	natl t = ckasyncread(0, t05v0, 19, 0);
	delay(5);
	ckwait(0, t, t05v0, "ABCDEFGHIJKLMNOPQRS", 19);
	testok(05);
	end_test();
}

void t05p1b(natq test_num)
{
	natl t = ckasyncread(1, t05v1, 19, 0);
	ckwait(1, t, t05v1, "ABCDEFGHIJKLMNOPQRS", 19);
	testok(05);
	end_test();
}

extern natl mainp;
void main_body(natq id)
{
	natl prio = 600;

	end_test = sem_ini(0);

	test_num = 0;
	dbg(">>>INIZIO<<<: errori vari");
	new_proc(00, 0);
	new_proc(00, 1);
	new_proc(00, 2);
	new_proc(00, 3);
	new_proc(00, 4);
	delay(10);
	dbg("=== FINE ===");

	test_num = 1;
	dbg(">>>INIZIO<<<: caso base");
	new_proc(01, 0);
	sem_wait(end_test);
	if (t01n0 == 1) msg("OK");
	dbg("=== FINE ===");

	test_num = 2;
	dbg(">>>INIZIO<<<: due pendenti");
	new_proc(02, 0);
	sem_wait(end_test);
	if (t02n0 == 1) msg("OK");
	dbg("=== FINE ===");

	test_num = 3;
	dbg(">>>INIZIO<<<: troppe richieste");
	new_proc(03, 0);
	sem_wait(end_test);
	if (t03n0 == 1) msg("OK");
	dbg("=== FINE ===");

	test_num = 4;
	dbg(">>>INIZIO<<<: più processi");
	new_proc(04, 0);
	new_proc(04, 1);
	sem_wait(end_test);
	sem_wait(end_test);
	if (t04n0 == 2) msg("OK");
	dbg("=== FINE ===");

	test_num = 5;
	dbg(">>>INIZIO<<<: più periferiche");
	new_proc(05, 0);
	new_proc(05, 1);
	sem_wait(end_test);
	sem_wait(end_test);
	if (t05n0 == 2) msg("OK");
	dbg("=== FINE ===");

	pause();
	terminate_p();
}
natl mainp;

extern "C" void main()
{
	mainp = activate_p(main_body, 0, 900, LIV_UTENTE);

	terminate_p();
}
