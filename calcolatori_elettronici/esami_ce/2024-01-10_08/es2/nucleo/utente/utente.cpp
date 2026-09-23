#include <all.h>

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

static inline const char* bool2str(natq v)
{
	static char buf[100];
	switch(v) {
	case false:
		return "false";
	case true:
		return "true";
	default:
		snprintf(buf, 100, "%lu", v);
		return buf;
	}
}

#define ckread(id_, buf_, n_, to_, ebuf_, en_, er_) do {\
	natq r_ = ceread_n_to(id_, buf_, n_, to_);\
	int nerr_ = 0;\
	if ((n_) != (en_)) {\
		err("letti %lu byte invece di %u", n_, en_);\
		nerr_++;\
	} else {\
		for (natl i_ = 0; i_ < (n_); i_++) {\
			if ((buf_)[i_] != (ebuf_)[i_]) {\
				err("byte %d: atteso %02x, ottenuto %02x",\
					i_, (buf_)[i_], (ebuf_)[i_]);\
				nerr_++;\
			}\
		}\
	}\
	if ((er_) != r_) {\
		err("ottenuto %s invece di %s",\
			bool2str(r_), bool2str(er_));\
		nerr_++;\
	}\
	if (nerr_)\
		goto error;\
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
char t00v0[100];
natq t00v1 = 100;
const char t00v2[100] = { 0 };
const natq t00v3 = 100;

void t00p0b(natq test_num)
{
	ceread_n_to(1, t00v0, t00v1, 100);
	err("accesso a dispositivo non valido non ha causato abort");
	terminate_p();
}

void t00p1b(natq test_num)
{
	ceread_n_to(0, nullptr, t00v1, 100);
	err("buffer non valido non ha causato abort");
	terminate_p();
}

void t00p2b(natq test_num)
{
	natq *p = nullptr;
	ceread_n_to(0, t00v0, *p, 100);
	err("riferimento non valido non ha causato abort");
	terminate_p();
}

void t00p3b(natq test_num)
{
	ceread_n_to(0, t00v0, t00v1, 0);
	err("time-out nullo non ha causato abort");
	terminate_p();
}

void t00p4b(natq test_num)
{
	ceread_n_to(0, const_cast<char*>(t00v2), t00v1, 100);
	err("buffer non scrivibile non ha causato abort");
	terminate_p();
}

void t00p5b(natq test_num)
{
	natq *p = const_cast<natq*>(&t00v3);
	ceread_n_to(0, const_cast<char*>(t00v0), *p, 100);
	err("quanti non scrivibile non ha causato abort");
	terminate_p();
}

///**********************************************************************
// *             test 01: nessun time out                               *
// **********************************************************************/

natl t01p0;
char t01v0[5];
natq t01v1;
TCNT(01);

void t01p0b(natq test_num)
{
	cedebug(0, "ABCDE", 5);
	t01v1 = 5;
	ckread(0, t01v0, t01v1, 100, "ABCDE", 5, true);
	testok(01);
	end_test();
}


///**********************************************************************
// *             test 02: time out per mancanza dati                    *
// **********************************************************************/

natl t02p0;
char t02v0[5];
natq t02v1;
TCNT(02);

void t02p0b(natq test_num)
{
	t02v1 = 5;
	ckread(0, t02v0, t02v1, 4, "", 0, false);
	testok(02);
	end_test();
}

///**********************************************************************
// *             test 03: time out con dati parziali                    */
// **********************************************************************/

natl t03p0;
char t03v0[5];
natq t03v1;
TCNT(03);

void t03p0b(natq test_num)
{
	cedebug(0, "1234", 4);
	t03v1 = 5;
	ckread(0, t03v0, t03v1, 4, "1234", 4, false);
	testok(03);
	end_test();
}

///**********************************************************************
// *             test 04: time out per periferica occupata              * 
// **********************************************************************/

natl t04p0;
natl t04p1;
natl t04p2;
char t04v0[8];
natq t04v1;
char t04v2[4];
natq t04v3;
TCNT(04);

void t04p0b(natq test_num)
{
	cedebug(0, "abcd", 4);
	delay(10);
	cedebug(0, "efghEFGH", 8);
	testok(04);
	end_test();
}

void t04p1b(natq test_num)
{
	t04v1 = 8;
	ckread(0, t04v0, t04v1, 20, "abcdefgh", 8, true);
	testok(04);
	end_test();
}

void t04p2b(natq test_num)
{
	t04v3 = 4;
	ckread(0, t04v2, t04v3, 5, "", 0, false);
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
	new_proc(00, 0);
	new_proc(00, 1);
	new_proc(00, 2);
	new_proc(00, 3);
	new_proc(00, 4);
	new_proc(00, 5);
	delay(1);
	dbg("=== FINE ===");

	test_num = 1;
	dbg(">>>INIZIO<<<: nessun time out");
	new_proc(01, 0);
	sem_wait(end_test);
	if (t01n0 == 1) msg("OK");
	dbg("=== FINE ===");

	test_num = 2;
	dbg(">>>INIZIO<<<: time out per mancanza dati");
	new_proc(02, 0);
	sem_wait(end_test);
	if (t02n0 == 1) msg("OK");
	dbg("=== FINE ===");

	test_num = 3;
	dbg(">>>INIZIO<<<: time out con dati parziali");
	new_proc(03, 0);
	sem_wait(end_test);
	if (t03n0 == 1) msg("OK");
	dbg("=== FINE ===");

	test_num = 4;
	dbg(">>>INIZIO<<<: time out per periferica occupata");
	new_proc(04, 0);
	new_proc(04, 1);
	new_proc(04, 2);
	sem_wait(end_test);
	sem_wait(end_test);
	sem_wait(end_test);
	if (t04n0 == 3) msg("OK");
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
