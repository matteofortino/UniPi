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
natl t00p1;
natl t00p2;
natl t00s0;

void t00p0b(natq test_num)
{
	get_swap_ev();
	err("get_swap_ev() da processo non-swapper non ha causato abort");
	terminate_p();
}

void t00p1b(natq test_num)
{
	swapper();
	sem_wait(t00s0);
	swap_out(t00p2, stacks[0]);
	swap_in(t00p2, nullptr);
	err("swap_in() su processo rimosso e buf nullo non ha causato abort");
	terminate_p();
}

void t00p2b(natq test_num)
{
	sem_signal(t00s0);
	terminate_p();
}

///**********************************************************************
// *             test 01: caso base                                     *
// **********************************************************************/

natl t01p0;
TCNT(01);

void t01p0b(natq test_num)
{
	testok(01);
	end_test();
}

///**********************************************************************
// *             test 02: due processi                                  *
// **********************************************************************/

natl t02p0;
natl t02p1;
TCNT(02);

void t02p0b(natq test_num)
{
	testok(02);
	end_test();
}

void t02p1b(natq test_num)
{
	testok(02);
	end_test();
}

///**********************************************************************
// *             test 03: processo incompleto                           *
// **********************************************************************/

natl t03p0;
natl t03p1;
natl t03p2;
natl t03p3;
natl t03p4;
natl t03p5;
TCNT(03);

void t03p0b(natq test_num)
{
	testok(03);
	end_test();
}

void t03p1b(natq test_num)
{
	testok(03);
	end_test();
}

void t03p2b(natq test_num)
{
	testok(03);
	end_test();
}

void t03p3b(natq test_num)
{
	testok(03);
	end_test();
}

void t03p4b(natq test_num)
{
	testok(03);
	end_test();
}

void t03p5b(natq test_num)
{
	testok(03);
	end_test();
}

///**********************************************************************
// *             test 04: swap_in non richiesto                         *
// **********************************************************************/

natl t04p0;
natl t04p1;
natl t04p2;
natl t04p3;
TCNT(04);

void t04p0b(natq test_num)
{
	testok(04);
	end_test();
}

void t04p1b(natq test_num)
{
	testok(04);
	end_test();
}

void t04p2b(natq test_num)
{
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
	// test 1
	swap_out(t01p0, stacks[0]);
	sem_signal(acktest);
	if ( (rv = get_swap_ev()) != t01p0) {
		msg("get_swap_ev(): atteso %u, ricevuto %u", t01p0, rv);
		goto out;
	}
	if ( (rv = swap_in(t01p0, stacks[0])) != true) {
		msg("swap_in(): atteso true(%x), ricevuto %x", true, rv);
		goto out;
	}
	sem_wait(nexttest);
	// test 2
	swap_out(t02p1, stacks[0]);
	sem_signal(acktest);
	if ( (rv = get_swap_ev()) != t02p1) {
		msg("get_swap_ev(): atteso %u, ricevuto %u", t02p1, rv);
		goto out;
	}
	if ( (rv = swap_in(t02p1, stacks[0])) != true) {
		msg("swap_in(): atteso true(%x), ricevuto %x", true, rv);
		goto out;
	}
	sem_wait(nexttest);
	// test 3
	if ( (rv = get_swap_ev()) != t03p5) {
		msg("get_swap_ev(): atteso %u, ricevuto %u", t03p5, rv);
		goto out;
	}
	if ( (rv = swap_in(t03p5, nullptr)) != true) {
		msg("swap_in(): atteso true(%x), ricevuto %x", true, rv);
		goto out;
	}
	sem_signal(acktest);
	sem_wait(nexttest);
	// test 4
	swap_out(t04p0, stacks[0]);
	swap_out(t04p1, stacks[1]);
	swap_out(t04p2, stacks[2]);
	sem_signal(acktest);
	if ( (rv = get_swap_ev()) != t04p0) {
		msg("get_swap_ev(): atteso %u, ricevuto %u", t04p0, rv);
		goto out;
	}
	if ( (rv = swap_in(t04p0, stacks[0])) != true) {
		msg("swap_in(): atteso true(%x), ricevuto %x", true, rv);
		goto out;
	}
	if ( (rv = swap_in(t04p0, nullptr)) != true) {
		msg("swap_in(): atteso true(%x), ricevuto %x", true, rv);
		goto out;
	}
	if ( (rv = swap_in(t04p1, stacks[1])) != true) {
		msg("swap_in(): atteso true(%x), ricevuto %x", true, rv);
		goto out;
	}
	if ( (rv = get_swap_ev()) != t04p2) {
		msg("get_swap_ev(): atteso %u, ricevuto %u", t04p2, rv);
		goto out;
	}
	if ( (rv = swap_in(t04p2, stacks[2])) != true) {
		msg("swap_in(): atteso true(%x), ricevuto %x", true, rv);
		goto out;
	}
	sem_wait(nexttest);
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
	t00s0 = sem_ini(0);
	new_proc(00, 0);
	new_proc(00, 1);
	new_proc(00, 2);
	delay(10);
	dbg("=== FINE ===");

	if (activate_p(myswapper, nexttest, 700, LIV_UTENTE) == 0xFFFFFFFF) {
		msg("Impossibile creare il processo swapper");
		pause();
		terminate_p();
	}

	test_num = 1;
	dbg(">>>INIZIO<<<: caso base");
	new_proc(01, 0);
	sem_signal(nexttest);
	sem_wait(acktest);
	sem_wait(end_test);
	if (t01n0 == 1) msg("OK");
	delay(10);
	dbg("=== FINE ===");

	test_num = 2;
	dbg(">>>INIZIO<<<: due processi");
	new_proc(02, 0);
	new_proc(02, 1);
	sem_signal(nexttest);
	sem_wait(acktest);
	sem_wait(end_test);
	sem_wait(end_test);
	if (t02n0 == 2) msg("OK");
	delay(10);
	dbg("=== FINE ===");

	test_num = 3;
	dbg(">>>inizio<<<: processo incompleto");
	new_proc(03, 0);
	new_proc(03, 1);
	new_proc(03, 2);
	new_proc(03, 3);
	new_proc(03, 4);
	new_proc(03, 5);
	sem_wait(end_test);
	sem_wait(end_test);
	sem_wait(end_test);
	sem_wait(end_test);
	sem_wait(end_test);
	sem_signal(nexttest);
	sem_wait(acktest);
	sem_wait(end_test);
	if (t03n0 == 6) msg("OK");
	delay(10);
	dbg("=== fine ===");

	test_num = 4;
	dbg(">>>inizio<<<: swap_in non richiesto");
	new_proc(04, 0);
	new_proc(04, 1);
	new_proc(04, 2);
	sem_signal(nexttest);
	sem_wait(acktest);
	sem_wait(end_test);
	sem_wait(end_test);
	sem_wait(end_test);
	if (t04n0 == 3) msg("OK");
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
