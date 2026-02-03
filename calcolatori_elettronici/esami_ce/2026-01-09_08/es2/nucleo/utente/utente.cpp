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

natq test_num;

natl sem0;
natl sem1;
natl sem2;
natl sem3;
natl sem4;
///**********************************************************************
// *             test 00: errori vari                                   *
// **********************************************************************/

natl t00p0;
natl t00p1;
natl t00p2;
natl t00p3;
natl t00p4;

void t00p0b(natq test_num)
{
	sem_multiwait(nullptr, MAX_MULTIWAIT);
	err("sem_multiwait su puntatore nullo non ha causato abort");
	terminate_p();
}

void t00p1b(natq test_num)
{
	natl sems[] = { sem0, 1024 };
	sem_multiwait(sems, 2);
	err("sem_multiwait con semafori non validi non ha causato abort");
	terminate_p();
}

void t00p2b(natq test_num)
{
	natl sems[] = { sem0, sem1, sem2, sem3, sem4 };
	sem_multiwait(sems, 5);
	err("sem_multiwati con numero di semafori non valido non ha causato abort");
	terminate_p();
}

///**********************************************************************
// *             test 01: sem_wait continua a funzionare                *
// **********************************************************************/

natl t01p0;
natl t01p1;
TCNT(01);

void t01p0b(natq test_num)
{
	sem_wait(sem0);
	testok(01);
	end_test();
}

void t01p1b(natq test_num)
{
	sem_signal(sem0);
	testok(01);
	end_test();
}

///**********************************************************************
// *             test 02: sem_multiwait caso base                       *
// **********************************************************************/

natl t02p0;
natl t02p1;
TCNT(02);

void t02p0b(natq test_num)
{
	natl sems[] = { sem0, sem1 };
	sem_multiwait(sems, 2);
	testok(02);
	end_test();
}

void t02p1b(natq test_num)
{
	sem_signal(sem0);
	sem_signal(sem1);
	testok(02);
	end_test();
}

///**********************************************************************
// *             test 03: sem_multiwait con semafori disponibili        *
// **********************************************************************/

natl t03p0;
natl t03p1;
natl t03p2;
TCNT(03);

void t03p0b(natq test_num)
{
	sem_signal(sem1);
	sem_wait(sem3);
	testok(03);
	end_test();
}

void t03p1b(natq test_num)
{
	natl sems[] = { sem0, sem1, sem2 };
	sem_multiwait(sems, 3);
	sem_signal(sem3);
	testok(03);
	end_test();
}

void t03p2b(natq test_num)
{
	sem_wait(sem1);
	sem_signal(sem1);
	sem_signal(sem0);
	sem_signal(sem2);
	testok(03);
	end_test();
}

///**********************************************************************
// *             test 04: sem_signal con processi ombra e non           *
// **********************************************************************/

natl t04p0;
natl t04p1;
natl t04p2;
natl t04p3;
TCNT(04);

void t04p0b(natq test_num)
{
	natl sems[] = { sem0, sem1 };
	sem_multiwait(sems, 2);
	sem_signal(sem1);
	testok(04);
	end_test();
}

void t04p1b(natq test_num)
{
	natl sems[] = { sem1, sem2 };
	sem_multiwait(sems, 2);
	testok(04);
	end_test();
}

void t04p2b(natq test_num)
{
	sem_wait(sem1);
	sem_signal(sem1);
	sem_signal(sem0);
	testok(04);
	end_test();
}

void t04p3b(natq test_num)
{
	sem_signal(sem1);
	testok(04);
	end_test();
}

extern natl mainp;
void main_body(natq id)
{
	natl prio = 600;

	end_test = sem_ini(0);
	natl membefore = getmeminfo().heap_libero;

	test_num = 0;
	dbg(">>>INIZIO<<<: errori vari");
	new_proc(00, 0);
	new_proc(00, 1);
	new_proc(00, 2);
	delay(10);
	dbg("=== FINE ===");

	test_num++;
	dbg(">>>INIZIO<<<: sem_wait continua a funzionare");
	new_proc(01, 0);
	new_proc(01, 1);
	sem_wait(end_test);
	sem_wait(end_test);
	if (t01n0 == 2) msg("OK");
	dbg("=== FINE ===");

	test_num++;
	dbg(">>>INIZIO<<<: sem_multiwait caso base");
	new_proc(02, 0);
	new_proc(02, 1);
	sem_wait(end_test);
	sem_wait(end_test);
	if (t02n0 == 2) msg("OK");
	dbg("=== FINE ===");

	test_num++;
	dbg(">>>INIZIO<<<: sem_multiwait con semafori disponibili");
	new_proc(03, 0);
	new_proc(03, 1);
	new_proc(03, 2);
	sem_wait(end_test);
	sem_wait(end_test);
	sem_wait(end_test);
	if (t03n0 == 3) msg("OK");
	dbg("=== FINE ===");

	test_num++;
	dbg(">>>INIZIO<<<: sem_signal con processi ombra e non");
	new_proc(04, 0);
	new_proc(04, 1);
	new_proc(04, 2);
	new_proc(04, 3);
	sem_wait(end_test);
	sem_wait(end_test);
	sem_wait(end_test);
	sem_wait(end_test);
	if (t04n0 == 4) msg("OK");
	dbg("=== FINE ===");

	delay(10);
	test_num++;
	// controlliamo che tutta la memoria sia stata correttamente rilasciata
	natl memafter = getmeminfo().heap_libero;
	if (memafter != membefore)
		err("memoria non liberata: %d frame", membefore - memafter);
	pause();
	terminate_p();
}
natl mainp;

extern "C" void main()
{
	sem0 = sem_ini(0);
	sem1 = sem_ini(0);
	sem2 = sem_ini(0);
	sem3 = sem_ini(0);
	sem4 = sem_ini(0);
	mainp = activate_p(main_body, 0, 900, LIV_UTENTE);

	terminate_p();
}
