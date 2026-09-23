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
	t##n##n0++;\
} while (0)

#define ckget(r_, e_) do {\
	bool v_ = cp_get(r_);\
	if (v_ != (e_)) {\
		err("ottenuto %x invece di %x", v_, e_);\
		goto error;\
	}\
} while (0)

natq test_num;
static const natq PART_SIZE = dim_region(MAX_LIV - 1);
const vaddr ini_utn_p = norm(I_UTN_P * PART_SIZE);
const vaddr ini_sis_p = norm(I_SIS_P * PART_SIZE); ///< base di sistema/privata
const vaddr fin_utn_p = ini_utn_p + PART_SIZE * N_UTN_P;
const vaddr fin_sis_p = ini_sis_p + PART_SIZE * N_SIS_P; ///< limite di sistema/privata
natq *start_of_stack = reinterpret_cast<natq*>(fin_utn_p - DIM_USR_STACK);

#define setstack(m_, p_, v_) do {\
	natq *pv_ = static_cast<natq*>(m_);\
	pv_[(p_) - start_of_stack] = (v_);\
} while(0)

#define ckstack(m_, p_, e_) do {\
	natq *pv_ = static_cast<natq*>(m_);\
	natq v_ = pv_[(p_) - start_of_stack];\
	if (v_ != (e_)) {\
		die("ottenuto %016lx invece di %016lx", v_, e_);\
	}\
} while(0)

const char *regnames[] = { "rax", "rcx", "rdx", "rbx", "rsp", "rbp",
	"rsi", "rdi", "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15"};

#define ckreg(r_, i_, e_) do {\
	if ((r_)[i_] != (e_)) {\
		die("registro %s: ottenuto %016lx invece di %016lx",\
				regnames[i_], (r_)[i_], e_);\
	}\
} while(0)

///**********************************************************************
// *             test 00: errori vari                                   *
// **********************************************************************/

natl t00p0;
natl t00p1;
natl t00p2;
natl t00p3;
natl t00s0;
natq t00v1[N_REG + 1];
const natq t00v2[N_REG + 1] = {0};

void t00p0b(natq test_num)
{
	cp_get(t00v1);
	err("cp_get() senza essere master non ha causato abort");
	terminate_p();
}

void t00p1b(natq test_num)
{
	sem_wait(t00s0);
	terminate_p();
}

void t00p2b(natq test_num)
{
	cp_prep(t00p1);
	cp_get(nullptr);
	err("cp_get() con regs non valido non ha causato abort");
	terminate_p();
}

void t00p3b(natq test_num)
{
	cp_prep(t00p1);
	cp_get(const_cast<natq*>(t00v2));
	err("cp_get() con regs non valido non ha causato abort");
	terminate_p();
}

///**********************************************************************
// *             test 01: caso base                                     *
// **********************************************************************/

natl t01p0;
natl t01p1;
natl t01s0;
natq*t01v0;
natq t01v1[N_REG + 1];
const natq t01c0 = 0xaabbccddeeff1122;
TCNT(01);

void t01p0b(natq test_num)
{
	sem_wait(t01s0);
	void *m = cp_prep(t01p1);
	setstack(m, t01v0, 0);
	ckget(t01v1, true);
	ckstack(m, t01v0, t01c0);
	testok(01);
	end_test();
}

void t01p1b(natq test_num)
{
	natq local = t01c0;
	t01v0 = &local;
	sem_signal(t01s0);
	local++;
	testok(01);
	end_test();
}

///**********************************************************************
// *             test 02: sincronizzazione invertita                    *
// **********************************************************************/

natl t02p0;
natl t02p1;
natl t02s0;
natq*t02v0;
natq t02v1[N_REG + 1];
const natq t02c0 = 0x55aa55aa11223344;
TCNT(02);

void t02p0b(natq test_num)
{
	sem_wait(t02s0);
	void *m = cp_prep(t02p1);
	setstack(m, t02v0, 0);
	delay(1);
	ckget(t02v1, true);
	ckstack(m, t02v0, t02c0);
	testok(02);
	end_test();
}

void t02p1b(natq test_num)
{
	natq local = t02c0;
	t02v0 = &local;
	sem_signal(t02s0);
	local++;
	testok(02);
	end_test();
}

///**********************************************************************
// *             test 03: registri                                      *
// **********************************************************************/

natl t03p0;
natl t03p1;
natl t03s0;
natq*t03v0;
natq t03v1[N_REG + 1];
const natq t03c0 = 0xdeadbeefdeadbeef;
TCNT(03);

void t03p0b(natq test_num)
{
	sem_wait(t03s0);
	cp_prep(t03p1);
	memset(t03v1, 0, sizeof(t03v1));
	ckget(t03v1, true);
	ckreg(t03v1, I_R12, t03c0);
	testok(03);
	end_test();
}

void t03p1b(natq test_num)
{
	asm volatile (
		"mov %0, %%r12\n\t"
		"mov %1, %%edi\n\t"
		"int %2\n\t"
		"mov %3, %%r12"
		:
		: "i" (t03c0),
		  "r" (t03s0),
		  "N" (TIPO_S),
		  "i" (t03c0+1)
		: "r12");
	testok(03);
	end_test();
}

///**********************************************************************
// *             test 04: registri con sincronizzazione invertita       *
// **********************************************************************/

natl t04p0;
natl t04p1;
natl t04s0;
natq*t04v0;
natq t04v1[N_REG + 1];
const natq t04c0 = 0xcafebabecafebabe;
TCNT(04);

void t04p0b(natq test_num)
{
	sem_wait(t04s0);
	cp_prep(t04p1);
	memset(t04v1, 0, sizeof(t04v1));
	delay(1);
	ckget(t04v1, true);
	ckreg(t04v1, I_R12, t04c0);
	testok(04);
	end_test();
}

void t04p1b(natq test_num)
{
	asm volatile ("mov %0, %%r12\n\tmov %1, %%edi\n\tint %2"
			: : "i" (t04c0), "r" (t04s0), "N" (TIPO_S) : "r12");
	testok(04);
	end_test();
}

///**********************************************************************
// *             test 05: stack grande                                  *
// **********************************************************************/

natl t05p0;
natl t05p1;
natl t05s0;
natq*t05v0;
natq*t05v1;
natq t05v2[N_REG + 1];
const natq t05c0 = 0x0123456789abcdef;
const natq t05c1 = 0xfedcba9876543210;
TCNT(05);

void t05p0b(natq test_num)
{
	sem_wait(t05s0);
	void *m = cp_prep(t05p1);
	setstack(m, t05v0, 0);
	setstack(m, t05v1, 0);
	ckget(t05v2, true);
	ckstack(m, t05v0, t05c0);
	ckstack(m, t05v1, t05c1);
	testok(05);
	end_test();
}

void t05p1b(natq test_num)
{
	natq local[2048];
	t05v0 = &local[111];
	t05v1 = &local[1024+37];
	*t05v0 = t05c0;
	*t05v1 = t05c1;
	sem_signal(t05s0);
	testok(05);
	end_test();
}

///**********************************************************************
// *             test 06: stack grande con sincronizzazione invertita   *
// **********************************************************************/

natl t06p0;
natl t06p1;
natl t06s0;
natq*t06v0;
natq*t06v1;
natq t06v2[N_REG + 1];
const natq t06c0 = 0x0123456789abcdef;
const natq t06c1 = 0xfedcba9876543210;
TCNT(06);

void t06p0b(natq test_num)
{
	sem_wait(t06s0);
	void *m = cp_prep(t06p1);
	setstack(m, t06v0, 0);
	setstack(m, t06v1, 0);
	delay(1);
	ckget(t06v2, true);
	ckstack(m, t06v0, t06c0);
	ckstack(m, t06v1, t06c1);
	testok(06);
	end_test();
}

void t06p1b(natq test_num)
{
	natq local[2048];
	t06v0 = &local[42];
	t06v1 = &local[1024+313];
	*t06v0 = t06c0;
	*t06v1 = t06c1;
	sem_signal(t06s0);
	testok(06);
	end_test();
}

// **********************************************************************/
// *             test 07: master che termina prematuramente		*
// **********************************************************************/

natl t07p0;
natl t07p1;
natl t07s0;
TCNT(07);

void t07p0b(natq test_num)
{
	sem_wait(t07s0);
	cp_prep(t07p1);
	testok(07);
	end_test();
}

void t07p1b(natq test_num)
{
	sem_signal(t07s0);
	testok(07);
	end_test();
}

///**********************************************************************
// *             test 08: ritorno intermedio                            *
// **********************************************************************/

natl t08p0;
natl t08p1;
natq t08v1[N_REG + 1];
char t08v2[512];
TCNT(08);

void t08p0b(natq test_num)
{
	readhd_n(t08v2, 0, 1);
	delay(1);
	testok(08);
	end_test();
}

void t08p1b(natq test_num)
{
	cp_prep(t08p0);
	ckget(t08v1, true);
	ckreg(t08v1, I_RSP, fin_utn_p - 0x18);
	testok(08);
	end_test();
}

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
	new_proc(00, 3);
	delay(10);
	sem_signal(t00s0);
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
	dbg(">>>INIZIO<<<: sincronizzazione invertita");
	t02s0 = sem_ini(0);
	new_proc(02, 0);
	new_proc(02, 1);
	sem_wait(end_test);
	sem_wait(end_test);
	if (t02n0 == 2) msg("OK");
	dbg("=== FINE ===");

	test_num = 3;
	dbg(">>>INIZIO<<<: registri");
	t03s0 = sem_ini(0);
	new_proc(03, 0);
	new_proc(03, 1);
	sem_wait(end_test);
	sem_wait(end_test);
	if (t03n0 == 2) msg("OK");
	dbg("=== FINE ===");

	test_num = 4;
	dbg(">>>INIZIO<<<: registri con sincronizzazione invertita");
	t04s0 = sem_ini(0);
	new_proc(04, 0);
	new_proc(04, 1);
	sem_wait(end_test);
	sem_wait(end_test);
	if (t04n0 == 2) msg("OK");
	dbg("=== FINE ===");

	test_num = 5;
	dbg(">>>INIZIO<<<: stack grande");
	t05s0 = sem_ini(0);
	new_proc(05, 0);
	new_proc(05, 1);
	sem_wait(end_test);
	sem_wait(end_test);
	if (t05n0 == 2) msg("OK");
	dbg("=== FINE ===");

	test_num = 6;
	dbg(">>>INIZIO<<<: stack grande con sincronizzazione invertita");
	t06s0 = sem_ini(0);
	new_proc(06, 0);
	new_proc(06, 1);
	sem_wait(end_test);
	sem_wait(end_test);
	if (t06n0 == 2) msg("OK");
	dbg("=== FINE ===");

	test_num = 7;
	dbg(">>>INIZIO<<<: master che termina prematuramente");
	t07s0 = sem_ini(0);
	new_proc(07, 0);
	new_proc(07, 1);
	sem_wait(end_test);
	sem_wait(end_test);
	if (t07n0 == 2) msg("OK");
	dbg("=== FINE ===");

	test_num = 8;
	dbg(">>>INIZIO<<<: ritorno intermedio");
	new_proc(08, 0);
	new_proc(08, 1);
	sem_wait(end_test);
	sem_wait(end_test);
	if (t08n0 == 2) msg("OK");
	dbg("=== FINE ===");

	pause();
	terminate_p();
}
