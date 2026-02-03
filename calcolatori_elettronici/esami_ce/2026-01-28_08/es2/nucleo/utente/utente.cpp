#include <all.h>
#include <vm.h>

#define dbg(s, ...) flog(LOG_DEBUG, "TEST %lu: " s, test_num, ## __VA_ARGS__)
#define msg(s, ...) printf("TEST %lu PROC %u: " s "\n", test_num, getpid(), ## __VA_ARGS__)

#define err(s, ...) msg("ERRORE: " s, ## __VA_ARGS__);

#define die(s, ...) do { err(s, ## __VA_ARGS__); goto error; } while (0)

#define new_proc(tn, pn)\
	t##tn##p##pn = activate_p(t##tn##p##pn##b, test_num, prio--, LIV_UTENTE);

natl end_test; // sync

#define end_test() do {\
	(void)&&error;\
error:\
	sem_signal(end_test);\
	terminate_p();\
} while (0)

#define TPH(n) t##n##f0
#define TCNT(n)	natl t##n##m0; natl t##n##n0; volatile natl TPH(n);
#define testok(n) do {\
	sem_wait(t##n##m0);\
	t##n##n0++;\
	sem_signal(t##n##m0);\
} while (0)
#define waitphase(n, f) do {\
	natq cnt_ = 0;\
	while (TPH(n) < (f)) {\
		if (++cnt_ % 32 == 0)\
			dbg("waitphase: %u != %u", TPH(n), f);\
		delay(1);\
	}\
} while (0)
#define chkphase(n, s, f) do {\
	natl f_ = TPH(n);\
	if (f_ != (f))\
		die("%s: fase %u invece di %u", s, f_, f);\
} while (0);
#define minphase(n, s, f) do {\
	natl f_ = TPH(n);\
	if (f_ < (f))\
		die("%s: troppo presto: fase %u invece di %u", s, f_, f);\
} while (0);

natq test_num;

// rw
natl rw0;

////////////////////////////////////////////////////////////////////////////////
// TEST 01: writer prima dei lettori "nuovi"
////////////////////////////////////////////////////////////////////////////////
//
// Scenario:
//  R0 prende READ e tiene
//  W1 prova WRITE (attesa)
//  R1 e R2 provano READ mentre W1 attende (devono attendere)
//  quando R0 esce: deve entrare W1 prima di R1/R2
//
// Fasi:
// 1: R0 dentro
// 2: W1 ha iniziato (sta per bloccare su acq_write)
// 3: R1 ha iniziato (sta per bloccare su acq_read)
// 4: R2 ha iniziato (sta per bloccare su acq_read)
// 5: R0 esce (libera)
// 6: W1 dentro (DEVE essere il primo)
// 7: W1 esce
//
////////////////////////////////////////////////////////////////////////////////

natl t01p0, t01p1, t01p2, t01p3;
TCNT(01);

void t01p0b(natq test_num) // R0
{
	rw_acq_read(rw0);
	dbg("R0 dentro");
	chkphase(01, "R0 dentro", 0);
	TPH(01) = 1;
	// aspetta che W1,R1,R2 abbiano dichiarato fino a phase==4
	waitphase(01, 4);
	dbg("R0 esce");
	chkphase(01, "R0 esce", 4);
	TPH(01) = 5;
	rw_rel_read(rw0);
	testok(01);
	end_test();
}

void t01p1b(natq test_num) // W1
{
	dbg("W1 prova WRITE (attesa)");
	chkphase(01, "W1 avvio", 1);
	TPH(01) = 2;
	rw_acq_write(rw0);
	dbg("W1 dentro");
	chkphase(01, "W1 dentro", 5);
	TPH(01) = 6;
	delay(5);
	dbg("W1 esce");
	chkphase(01, "W1 esce", 6);
	TPH(01) = 7;
	rw_rel_write(rw0);
	testok(01);
	end_test();
}

void t01p2b(natq tn) // R1
{
	// aspetta che W1 abbia dichiarato phase>=2
	waitphase(01, 2);
	dbg("R1 prova READ (attesa)");
	chkphase(01, "R1 avvio", 2);
	TPH(01) = 3;
	rw_acq_read(rw0);
	// deve entrare solo dopo che W1 ha finito (phase >= 7)
	minphase(01, "R1 dentro", 7);
	dbg("R1 dentro/esce");
	rw_rel_read(rw0);
	testok(01);
	end_test();
}

void t01p3b(natq tn) // R2
{
	// aspetta che R1 abbia dichiarato phase>=3
	waitphase(01, 3);
	dbg("R2 prova READ (attesa)");
	chkphase(01, "R2 avvio", 3);
	TPH(01) = 4;
	rw_acq_read(rw0);
	minphase(01, "R2 dentro", 7);
	dbg("R2 dentro/esce");
	rw_rel_read(rw0);
	testok(01);
	end_test();
}

////////////////////////////////////////////////////////////////////////////////
// TEST 02: alternanza a fasi W1 -> (R1,R2 batch) -> W2
////////////////////////////////////////////////////////////////////////////////
//
// Fasi:
// 0: W1 dentro
// 1: R1 ha iniziato (attesa)
// 2: R2 ha iniziato (attesa)
// 3: W2 ha iniziato (attesa)
// 4: W1 esce (libera)
// 5: R1 dentro (prima del writer2)
// 6: R2 dentro
// 7: W2 dentro (solo dopo che i lettori sono passati)
//
////////////////////////////////////////////////////////////////////////////////

natl t02p0, t02p1, t02p2, t02p3;
TCNT(02);

void t02p0b(natq test_num) // W1
{
	dbg("W1 prova WRITE");
	rw_acq_write(rw0);
	dbg("W1 dentro");
	chkphase(02, "W1 dentro", 0);
	TPH(02) = 1;
	// aspetta che R1,R2,W2 abbiano iniziato (phase==4)
	waitphase(02, 4);
	dbg("W1 esce");
	chkphase(02, "W1 esce", 4);
	TPH(02) = 5;
	rw_rel_write(rw0);
	testok(02);
	end_test();
}

void t02p1b(natq test_num) // R1
{
	dbg("R1 prova READ (attesa)");
	chkphase(02, "R1 avvio", 1);
	TPH(02) = 2;
	rw_acq_read(rw0);
	dbg("R1 dentro");
	chkphase(02, "R1 dentro", 5);
	TPH(02) = 6;
	delay(5);
	rw_rel_read(rw0);
	testok(02);
	end_test();
}

void t02p2b(natq test_num) // R2
{
	waitphase(02, 2);
	dbg("R2 prova READ (attesa)");
	chkphase(02, "R2 avvio", 2);
	TPH(02) = 3;
	rw_acq_read(rw0);
	dbg("R2 dentro");
	chkphase(02, "R2 dentro", 6);
	TPH(02) = 7;
	delay(5);
	rw_rel_read(rw0);
	testok(02);
	end_test();
}

void t02p3b(natq test_num) // W2
{
	waitphase(02, 3);
	dbg("W2 prova WRITE (attesa)");
	chkphase(02, "W2 avvio", 3);
	TPH(02) = 4;
	rw_acq_write(rw0);
	// deve entrare solo quando phase==7 (dopo ingresso lettori batch)
	chkphase(02, "W2 dentro", 7);
	dbg("W2 dentro/esce");
	rw_rel_write(rw0);
	testok(02);
	end_test();
}

////////////////////////////////////////////////////////////////////////////////
// TEST 03: i lettori devono aspettare se si raggiunge MAX_RW_READERS
////////////////////////////////////////////////////////////////////////////////
//
// Scenario:
//  - Avviamo MAX_RW_READERS lettori che acquisiscono READ e restano dentro.
//  - Avviamo un lettore EXTRA che prova ad acquisire READ: deve bloccarsi.
//  - Rilasciamo R0 (uno dei lettori già dentro) e verifichiamo che l'EXTRA
//    si sblocchi e acquisisca solo dopo.
//  - Poi rilasciamo tutti gli altri.
//
// Fasi:
// 0 .. MAX_RW_READERS-1 : acquisizioni dei lettori normali (in ordine di priorità)
// MAX_RW_READERS       : tutto pieno
// MAX_RW_READERS+1     : EXTRA ha "iniziato" (sta per bloccarsi su acq_read)
// MAX_RW_READERS+2     : R0 ha annunciato rilascio imminente (prima di liberare slot)
// MAX_RW_READERS+3     : EXTRA dentro (si e' sbloccato)
//
////////////////////////////////////////////////////////////////////////////////

natl t03p0, t03p1, t03p2, t03p3, t03p4;
TCNT(03);

void t03p0b(natq test_num) // R0
{
	// acquisisce READ: i primi MAX_RW_READERS non dovrebbero bloccare
	rw_acq_read(rw0);
	dbg("R0 dentro");
	chkphase(03, "R0 dentro", 0);
	TPH(03) = 1;
	waitphase(03, MAX_RW_READERS + 1);
	TPH(03) = MAX_RW_READERS + 2;
	dbg("R0 esce");
	rw_rel_read(rw0);
	testok(03);
	end_test();
}

void t03pXb(natq test_num, natq idx) // R1 .. R(MAX_RW_READERS-1)
{
	// acquisisce READ: i primi MAX_RW_READERS non dovrebbero bloccare
	rw_acq_read(rw0);
	dbg("R%lu dentro", idx);
	TPH(03) = idx + 1;
	waitphase(03, MAX_RW_READERS + 3);
	rw_rel_read(rw0);
	testok(03);
	end_test();
}

void t03p1b(natq test_num) { t03pXb(test_num, 1); }
void t03p2b(natq test_num) { t03pXb(test_num, 2); }
void t03p3b(natq test_num) { t03pXb(test_num, 3); }
void t03p4b(natq test_num) // R4 (EXTRA)
{
	// aspetta che tutti i reader normali abbiano riempito (fase == MAX_RW_READERS)
	waitphase(03, MAX_RW_READERS);
	dbg("R4 inizio");
	// dichiara che sta per bloccare su acq_read
	chkphase(03, "R4 inizio", MAX_RW_READERS);
	TPH(03) = MAX_RW_READERS + 1;
	// questo DEVE bloccare finché non viene liberato uno slot
	rw_acq_read(rw0);
	dbg("R4 dentro");
	// quando si sblocca, R0 deve aver già portato la fase a MAX_RW_READERS+2
	chkphase(03, "R4 dentro", MAX_RW_READERS + 2);
	TPH(03) = MAX_RW_READERS + 3;	
	// ora può rilasciare
	rw_rel_read(rw0);
	testok(03);
	end_test();
}


////////////////////////////////////////////////////////////////////////////////
// MAIN
////////////////////////////////////////////////////////////////////////////////

extern natl mainp;

void main_body(natq id)
{
	natl prio = 600;

	end_test = sem_ini(0);

	rw0 = rw_init();

	// ---------------- test 01 ----------------
	test_num = 1;
	dbg(">>>INIZIO<<<: writer prima dei lettori nuovi");

	new_proc(01, 0);
	new_proc(01, 1);
	new_proc(01, 2);
	new_proc(01, 3);

	sem_wait(end_test);
	sem_wait(end_test);
	sem_wait(end_test);
	sem_wait(end_test);

	if (t01n0 == 4) msg("OK");
	dbg("=== FINE ===");

	// ---------------- test 02 ----------------
	test_num = 2;
	dbg(">>>INIZIO<<<: alternanza W1->batch lettori->W2");

	new_proc(02, 0);
	new_proc(02, 1);
	new_proc(02, 2);
	new_proc(02, 3);

	sem_wait(end_test);
	sem_wait(end_test);
	sem_wait(end_test);
	sem_wait(end_test);

	if (t02n0 == 4) msg("OK");
	dbg("=== FINE ===");
	
	// ---------------- test 03 ----------------
	test_num = 3;
	dbg(">>>INIZIO<<<: i lettori devono aspettare se si raggiunge MAX_RW_READERS");

	new_proc(03, 0);
	new_proc(03, 1);
	new_proc(03, 2);
	new_proc(03, 3);
	new_proc(03, 4);

	sem_wait(end_test);
	sem_wait(end_test);
	sem_wait(end_test);
	sem_wait(end_test);
	sem_wait(end_test);

	if (t03n0 == 5) msg("OK");
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
